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

#include <stdio.h>
#include <stdint.h>
#include <wasm_export.h>
#include <wasm_c_api.h>
#include <lib_export.h>
#include "benchmarks.h"
#include "wasmtime_ssp.h"

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
uint32_t register_wasi();
int run_wasm()
{
	char error_buf[128];
	wasm_module_t module;
	wasm_module_inst_t module_inst;
	wasm_function_inst_t func;
	wasm_exec_env_t exec_env;
	uint32_t stack_size = 1 << 13;
	wasm_val_t results[1];
	size_t const results_len = 1;

	char const *mod = coremark_standalone;
	size_t const mod_size = sizeof coremark_standalone;

	/* initialize the wasm runtime by default configurations */
	RuntimeInitArgs runtime_args = {
		.mem_alloc_type = Alloc_With_System_Allocator,
		.running_mode = Mode_Interp,
	};
	wasm_runtime_full_init(&runtime_args);

	if (!register_wasi())
	{
		printf("Error registering native functions.\n");
		return 1;
	}
	/* parse the WASM file from buffer and create a WASM module */
	module = wasm_runtime_load(mod, mod_size, error_buf, sizeof(error_buf));

	/* create an instance of the WASM module (WASM linear memory is ready) */
	module_inst = wasm_runtime_instantiate(module, stack_size, 1 << 13,
										   error_buf, sizeof(error_buf));

	func = wasm_runtime_lookup_function(module_inst, "_initialize", NULL);
	exec_env = wasm_runtime_create_exec_env(module_inst, stack_size);
	if (func && !wasm_runtime_call_wasm_a(exec_env, func, 0, NULL, 0, NULL))
	{
		printf("error initializing wasm module!\n%s\n", wasm_runtime_get_exception(module_inst));
		return 1;
	}
	func = wasm_runtime_lookup_function(module_inst, "_run", NULL);
	if (!wasm_runtime_call_wasm_a(exec_env, func, results_len, results, 0, NULL))
	{
		printf("error executing wasm function!\n%s\n", wasm_runtime_get_exception(module_inst));
		return 1;
	}

	wasm_runtime_destroy_exec_env(exec_env);
	wasm_runtime_deinstantiate(module_inst);
	wasm_runtime_unload(module);
	wasm_runtime_destroy();
	return 0;
}

int main()
{
	if (pre_main())
	{
		return 1;
	}
	printf("CoreMark WAMR Example\n\n");
	run_wasm();
	return 0;
}