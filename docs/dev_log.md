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

### 2026-06-02 - Stage 2.1 本地视频路径配置读取

#### 今日完成

- [x] 新增视频源配置结构：`StreamConfig`、`StreamsConfig`。
- [x] 新增 `StreamConfigLoader::loadFromFile`，支持读取 `configs/streams.yaml`。
- [x] 支持解析 `id`、`name`、`type`、`url`、`enabled`、`loop`、`reconnect`、`reconnect_interval_ms`、`read_timeout_ms`、`expected_fps`。
- [x] 命令行新增 `--streams-config <path>`，默认读取 `configs/streams.yaml`。
- [x] 程序启动后打印启用的本地文件视频路径。

#### 验证命令

```bash
bash scripts/build/build.sh
bash scripts/run/run_local.sh --config configs/app.yaml --streams-config configs/streams.yaml
```

#### 验证结果

输出包含：

```text
[INFO] [main.cpp:48] loaded streams config: configs/streams.yaml
[INFO] [main.cpp:59] stream.file01.url = assets/videos/demo.mp4
[INFO] [main.cpp:62] local mp4 path = assets/videos/demo.mp4
```

#### Git 提交建议

- commit: `feat: load local video path from stream config`

### 2026-06-04 - Stage 2.1 OpenCV 本地视频读取 Demo

#### 今日完成

- [x] 按参考项目风格简化主流程：不新增复杂类，直接在 `main.cpp` 中实现 `readLocalVideoDemo()`。
- [x] 从 `configs/streams.yaml` 找到第一路启用的 `file` 视频源。
- [x] 使用 `cv::VideoCapture` 打开本地 mp4。
- [x] 打印视频宽、高、FPS、总帧数。
- [x] 读取视频帧，并每 30 帧打印一次读取进度。
- [x] 保存第一帧到 `output/frames/stage_2_1_first_frame.jpg`。
- [x] 拷贝测试视频到 `assets/videos/demo.mp4`，配置仍使用项目内相对路径。

#### 验证命令

```bash
bash scripts/build/build.sh
bash scripts/run/run_local.sh --config configs/app.yaml --streams-config configs/streams.yaml
```

#### 当前阶段说明

当前先完成前期代码和准备工作，后期再到 RK3588 开发板上验证运行。

当前 x86_64 环境缺少 OpenCV C++ 开发包，直接构建会停在 CMake 查找 OpenCV：

```text
Could not find a package configuration file provided by "OpenCV"
```

参考项目里的 OpenCV 是 ARM aarch64 版本，不能在当前 x86_64 环境直接链接。本阶段按代码准备完成处理，后续在 RK3588 板端把 OpenCV 放到 `third_party/opencv` 后再做实际运行验证。

#### Git 提交建议

- commit: `feat: add opencv local video reader demo`

### 2026-06-04 - Stage 2.2 视频帧缓冲模块

#### 今日完成

- [x] 新增 `Frame` 数据结构，包含图像、时间戳、通道 ID、帧序号。
- [x] 新增 `FrameBuffer`，用于保存和读取最新一帧。
- [x] `FrameBuffer::write()` 写入最新帧，内部复制图像，避免外部 `cv::Mat` 后续修改影响缓存。
- [x] `FrameBuffer::read()` 读取最新帧，内部复制图像，避免读写线程共用同一块图像内存。
- [x] 使用 `std::mutex` 保护读写，后续读线程和写线程可以安全访问。
- [x] 缓冲区只保留最新一帧，不使用队列，避免内存无限增长。
- [x] 在本地视频读取 Demo 中接入 `FrameBuffer`：每读到一帧先写入缓冲，再读取最新帧做保存和日志输出。

#### 当前阶段说明

当前先完成前期代码和准备工作。模块已经接入主流程，但完整多线程读写验证会在第 2.3 单路处理管线和 RK3588 环境中继续做。

当前 x86_64 环境仍缺少 OpenCV C++ 开发包，`bash scripts/build/build.sh` 会在 CMake 查找 OpenCV 时停止。后续在 RK3588 板端或已安装 OpenCV 的环境中继续编译运行验证。

#### Git 提交建议

- commit: `feat: add thread-safe frame buffer`

### 2026-06-04 - Stage 2.3 单路处理管线

#### 今日完成

- [x] 将本地视频读取 Demo 改成单路处理管线。
- [x] 主线程负责读取配置、初始化日志、注册 Ctrl+C 信号、启动和回收线程。
- [x] 新增读取线程：使用 `cv::VideoCapture` 读取本地视频帧，并写入 `FrameBuffer`。
- [x] 新增处理线程：从 `FrameBuffer` 读取最新帧，保存首帧，并打印处理到的帧序号。
- [x] 支持 `SIGINT` / `SIGTERM` 退出，按 Ctrl+C 后设置停止标志，两个线程正常退出并 join。
- [x] 支持配置中的 `loop`：如果 `loop: true`，视频读到末尾后从头继续读；否则读完后退出。

#### 当前阶段说明

当前先完成前期代码和准备工作。由于当前 x86_64 环境仍缺少 OpenCV C++ 开发包，构建会停在 CMake 查找 OpenCV；后续在 RK3588 板端或已安装 OpenCV 的环境中继续编译运行验证。

#### Git 提交建议

- commit: `feat: add single-stream processing pipeline`
