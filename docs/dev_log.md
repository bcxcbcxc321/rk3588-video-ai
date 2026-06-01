# 开发日志

## 模板

### YYYY-MM-DD

#### 今日完成

- [ ] 

#### 遇到的问题

- 

#### 解决方案

- 

#### 明日计划

- [ ] 

#### Git 提交

- commit: 

### 2026-06-01

#### 今日完成

- [x] 验证第 1 阶段 1.1：CMake 工程可配置、可编译、可运行。
- [x] 新增基础日志模块：`include/logging/logger.h`、`src/logging/logger.cpp`。
- [x] 新增本地构建/运行脚本：`scripts/build/build.sh`、`scripts/run/run_local.sh`、`scripts/build/clean.sh`。
- [x] 修正 `.gitignore`，避免误忽略 `scripts/build/` 目录。

#### 验证结果

```bash
./scripts/build/build.sh && ./scripts/run/run_local.sh
```

输出包含：

```text
[INFO] [main.cpp:7] RK3588 Video AI project skeleton started
[INFO] [main.cpp:8] Stage 1.1 validation: executable is running
```

#### Git 提交建议

- commit: `build: add initial cmake project skeleton and build scripts`

### 2026-06-01 - Stage 1.2 日志模块

#### 今日完成

- [x] 封装日志宏：`LOG_DEBUG`、`LOG_INFO`、`LOG_WARN`、`LOG_ERROR`。
- [x] 日志输出包含时间、等级、文件名、行号、消息内容。
- [x] 支持控制台输出。
- [x] 支持文件输出：通过环境变量 `RK_VIDEO_AI_LOG_FILE` 指定日志文件。
- [x] 支持日志等级：通过环境变量 `RK_VIDEO_AI_LOG_LEVEL=debug|info|warn|error` 指定。
- [x] 程序启动、退出、示例 warning/error 均能打印清晰日志。

#### 验证命令

```bash
./scripts/build/build.sh
RK_VIDEO_AI_LOG_LEVEL=debug RK_VIDEO_AI_LOG_FILE=output/logs/stage_1_2.log ./scripts/run/run_local.sh
tail -n 10 output/logs/stage_1_2.log
```

#### 验证结果

输出包含：

```text
[INFO] [main.cpp:10] RK3588 Video AI project skeleton started
[DEBUG] [main.cpp:11] debug log is enabled
[WARN] [main.cpp:12] configuration module is not implemented yet; using skeleton defaults
[ERROR] [main.cpp:13] sample error log for Stage 1.2 validation; no real failure occurred
[INFO] [main.cpp:14] RK3588 Video AI project skeleton exited normally
```

#### Git 提交建议

- commit: `feat: add basic logging module`

### 2026-06-01 - Stage 1.3 配置模块

#### 今日完成

- [x] 选择配置格式：YAML。
- [x] 创建根配置文件：`configs/app.yaml`、`configs/streams.yaml`、`configs/models.yaml`、`configs/streaming.yaml`。
- [x] 实现 App 配置结构体：`AppConfig`。
- [x] 实现 App 配置读取器：`AppConfigLoader::loadFromFile`。
- [x] 支持命令行参数：`--config <path>` 和 `--help`。
- [x] 支持配置校验：`app.name`、`app.log_level`、`app.enable_console_log`、`app.graceful_shutdown`。
- [x] 配置读取失败、配置值非法时能输出明确错误。

#### 验证命令

```bash
./scripts/build/build.sh
./scripts/run/run_local.sh --config configs/app.yaml
./scripts/run/run_local.sh --config configs/not_exists.yaml
./scripts/run/run_local.sh --help
```

#### 验证结果

正常配置输出包含：

```text
[INFO] [main.cpp:37] loaded config: configs/app.yaml
[INFO] [main.cpp:38] app.name = rk3588_video_ai
[INFO] [main.cpp:39] app.log_level = INFO
[INFO] [main.cpp:41] app.log_file = output/logs/app.log
```

异常配置输出包含：

```text
[ERROR] [main.cpp:24] config file not found or cannot be opened: configs/not_exists.yaml
[ERROR] [main.cpp:24] invalid app.log_level: verbose (expected debug/info/warn/error)
```

#### Git 提交建议

- commit: `feat: add yaml app configuration loader`
