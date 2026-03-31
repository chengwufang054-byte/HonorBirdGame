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

\- 尝试加入鼠标拖拽发射原型

\- 新增 birdStartPos、isDragging、maxDragDistance、launchPower 等拖拽相关变量

\- 实现鼠标按下、鼠标移动、鼠标松开三段基础拖拽逻辑

\- 保留空格发射作为当前稳定的备用发射方式

\- 在测试中发现拖拽发射与起点高度、地面判定、发射力度之间存在手感和逻辑冲突

\- 当前将拖拽发射视为“原型功能”，后续继续调整参数和布局

\- 整理了拖拽发射逻辑笔记，明确了按下、拖动、松开发射的代码流程- 实现命中后窗口标题显示 “You Win!”



\## Day 5

\- 将鼠标拖拽发射从原型推进到更稳定的版本

\- 松手时根据拖拽偏移计算速度，并从 birdStartPos 起点发射

\- 保留空格发射作为备用测试方式，拖拽发射作为主要玩法入口

\- 添加拖拽瞄准线，便于观察拖拽方向和力度

\- 限制拖拽方向，避免向起点右侧错误拖拽

\- 添加第二个 enemy 占位物

\- 实现第二个 enemy 的命中与消失逻辑

\- 将胜利条件改为：两个 enemy 都被清除后才显示 You Win!



\## Day 6

\- 为当前关卡加入 roundOver 回合结束状态

\- 实现本轮失败后显示 “Press R to Retry” 的逻辑

\- 让 roundOver 参与输入控制，避免回合结束后继续无效拖拽

\- 添加可摧毁障碍块 block，并实现碰撞后减速效果

\- 将障碍块升级为两段式耐久：第一次受损变深色，第二次被击碎

\- 加入画面内文字提示系统，显示发射、重试与胜利信息

\- 通过系统字体路径成功加载字体，完成游戏内基础 UI 文本显示



\## Day 7

\- 为关卡加入多只 bird 机制，使用 birdsLeft 表示剩余发射次数

\- 实现本轮结束后按 N 切换到下一只 bird

\- 将失败条件改为：bird 用完且目标未清除时按 R 重试

\- 调整关卡布局，使 enemy 位于横梁 block2 的正下方

\- 添加横梁 block2 下落逻辑，并在失去支撑后向下移动

\- 实现 enemy2 在横梁存在时被 block2 支撑，并随横梁一起下落

\- 实现 block2 被打碎后 enemy2 自己下落并摔死

\- 实现掉落中的 block2 砸死下方 enemy 的结构击杀效果

