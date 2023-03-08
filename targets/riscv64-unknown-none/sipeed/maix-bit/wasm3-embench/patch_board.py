Import("env")

board_config = env.BoardConfig()
board_config.update("upload.burn_tool", "goE")