/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "zephyr/kernel.h"
#include <zephyr/sys/printk.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/drivers/uart.h>

#include "m3_config.h"
#include "wasm3.h"
#include "m3_api_wasi.h"

#include "extra/fib32.wasm.h"
#include "benchmarks.h"

BUILD_ASSERT(DT_NODE_HAS_COMPAT(DT_CHOSEN(zephyr_console), zephyr_cdc_acm_uart),
			 "Console device is not ACM CDC UART device");

#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
USBD_CONFIGURATION_DEFINE(config_1,
						  USB_SCD_SELF_POWERED,
						  200);

USBD_DESC_LANG_DEFINE(sample_lang);
USBD_DESC_MANUFACTURER_DEFINE(sample_mfr, "ZEPHYR");
USBD_DESC_PRODUCT_DEFINE(sample_product, "Zephyr USBD ACM console");
USBD_DESC_SERIAL_NUMBER_DEFINE(sample_sn, "0123456789ABCDEF");

USBD_DEVICE_DEFINE(sample_usbd,
				   DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
				   0x2fe3, 0x0001);

static int enable_usb_device_next(void)
{
	int err;

	err = usbd_add_descriptor(&sample_usbd, &sample_lang);
	if (err)
	{
		return err;
	}

	err = usbd_add_descriptor(&sample_usbd, &sample_mfr);
	if (err)
	{
		return err;
	}

	err = usbd_add_descriptor(&sample_usbd, &sample_product);
	if (err)
	{
		return err;
	}

	err = usbd_add_descriptor(&sample_usbd, &sample_sn);
	if (err)
	{
		return err;
	}

	err = usbd_add_configuration(&sample_usbd, &config_1);
	if (err)
	{
		return err;
	}

	err = usbd_register_class(&sample_usbd, "cdc_acm_0", 1);
	if (err)
	{
		return err;
	}

	err = usbd_init(&sample_usbd);
	if (err)
	{
		return err;
	}

	err = usbd_enable(&sample_usbd);
	if (err)
	{
		return err;
	}

	return 0;
}
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK_NEXT) */

int pre_main(void)
{
	const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t dtr = 0;

#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
	if (enable_usb_device_next())
	{
		return 1;
	}
#else
	if (usb_enable(NULL))
	{
		return 1;
	}
#endif

	/* Poll if the DTR flag was set */
	while (!dtr)
	{
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		/* Give CPU resources to low priority threads. */
		k_sleep(K_MSEC(100));
	}

	printk("Console Ready\r\n");
	return 0;
}

#define FATAL(msg, ...)                            \
	{                                              \
		printf("Fatal: " msg "\n", ##__VA_ARGS__); \
		return;                                    \
	}
static int64_t start_msecs = 0;
static int64_t stop_msecs = 0;
static m3ApiRawFunction(get_time_wrapper)
{
	m3ApiReturnType(uint32_t)
		uint32_t res = stop_msecs - start_msecs;
	m3ApiReturn(res);
}
static m3ApiRawFunction(stop_time_wrapper)
{
	stop_msecs = k_uptime_get();
	m3ApiSuccess();
}
static m3ApiRawFunction(start_time_wrapper)
{
	start_msecs = k_uptime_get();
	m3ApiSuccess();
}
static m3ApiRawFunction(get_milsecs_wrapper)
{
	m3ApiReturnType(uint32_t)
		uint32_t result = k_uptime_get();
	m3ApiReturn(result);
}
void run_wasm()
{
	M3Result result = m3Err_none;

	uint8_t *wasm = coremark_standalone;
	uint32_t fsize = sizeof(coremark_standalone);

	printf("Loading WebAssembly...\n");
	IM3Environment env = m3_NewEnvironment();
	if (!env)
		FATAL("m3_NewEnvironment failed");

	printf("Creating Runtime\n");
	IM3Runtime runtime = m3_NewRuntime(env, 1 << 14, NULL);
	if (!runtime)
		FATAL("m3_NewRuntime failed");

	IM3Module module;
	printf("Parsing Module\r\n");
	result = m3_ParseModule(env, &module, wasm, fsize);
	if (result)
		FATAL("m3_ParseModule: %s", result);

	printf("Loading Module\r\n");
	result = m3_LoadModule(runtime, module);
	if (result)
		FATAL("m3_LoadModule: %s", result);
	    result = m3_LinkWASI(module);
    if (result)
    {
        FATAL("link WASI: %s\n", result);
    }
	(m3_LinkRawFunction(module, "env", "start_time", "()", &start_time_wrapper));
	(m3_LinkRawFunction(module, "env", "stop_time", "()", &stop_time_wrapper));
	(m3_LinkRawFunction(module, "env", "get_time", "i()", &get_time_wrapper));
	(m3_LinkRawFunction(module, "env", "get_milsecs", "i()", &get_milsecs_wrapper));
	IM3Function f;
	result = m3_FindFunction(&f, runtime, "_run");
	if (result)
		FATAL("m3_FindFunction: %s", result);

	printf("Running...\r\n");

	result = m3_CallV(f, 10);
	if (result)
		FATAL("m3_Call: %s", result);

	uint32_t value = 0;
	result = m3_GetResultsV(f, &value);
	if (result)
		FATAL("m3_GetResults: %s", result);

	printf("Result: %d\n", value);
}

int main()
{
	if (pre_main())
	{
		return 1;
	}
	printf("Wasm3 v" M3_VERSION " (" M3_ARCH "), build " __DATE__ " " __TIME__ "\n");
	run_wasm();
	return 0;
}