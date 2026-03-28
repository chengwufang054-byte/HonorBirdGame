\# 开发日志



\## Day 1

\- 明确项目方向为 C++ 图形化弹射闯关游戏

\- 确定项目名称为“荣耀小鸟”

\- 初始化项目目录结构（assets、docs、include、src、third\_party）

\- 初始化 Git 仓库

\- 配置 Git 用户名与邮箱

\- 创建并编写 .gitignore

\- 创建并编写 README.md 初版

\- 完成第一次提交：init: add gitignore and initial README

\- 创建报告骨架 report\_outline.md

\- 创建开发日志 dev\_log.md



\## Day 2

\- 在 Visual Studio 中创建 C++ 空项目 HonorBirdGame

\- 创建 main.cpp，并验证最小 C++ 程序能够成功生成

\- 下载并解压 SFML 2.6.1 到固定目录

\- 配置 SFML 的 include 路径

\- 配置 SFML 的 lib 路径

\- 配置链接器附加依赖项（graphics、window、system）

\- 复制运行所需的 SFML dll 到 Debug 输出目录

\- 成功运行第一个 SFML 窗口程序



\## Day 3

\- 为窗口添加天空背景色

\- 添加地面矩形，形成基础场景

\- 添加红色圆形 bird 作为小鸟占位物

\- 实现 bird 的左右移动

\- 为 bird 添加边界限制，避免跑出屏幕

\- 调整 bird 到发射区附近，并添加黑色轮廓

\- 添加简化版弹弓立柱 slingPost

\- 将 bird 的活动范围限制在发射区附近

\- 引入 birdReady 状态变量，表示是否处于待发射状态

\- 实现按空格切换 birdReady 为 false

\- 实现按 R 重置 bird 到初始位置，并恢复待发射状态



\## Day 4

\- 为 bird 增加水平速度 birdSpeedX，实现在发射后自动向右运动

\- 为 bird 增加竖直速度 birdSpeedY 和 gravity，形成基础抛物线雏形

\- 调整发射速度和重力参数，使飞行过程更容易观察

\- 实现 bird 落地后停止，不再继续穿过地面

\- 实现 bird 落地后变灰，作为发射结束的视觉提示

\- 添加 enemy 占位物，作为发射目标

\- 实现 bird 与 enemy 的命中判断

\- 实现 enemy 被命中后消失

\- 实现命中后窗口标题显示 “You Win!”

