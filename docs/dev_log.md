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



\## Day 8

\- 加入 GameState，形成 StartMenu / Playing / LevelClear / Win / Lose 的基本流程

\- 实现按 Enter 从开始界面进入游戏

\- 实现通关后进入 LevelClear 过渡界面，而不是直接瞬间切到下一关

\- 加入 currentLevel 与 nextLevel，完成最小双关卡系统

\- 实现 Level 1 通关后进入 Level 2，Level 2 通关后进入最终胜利界面

\- 将 LevelClear 文案改为动态显示当前关卡与下一关

\- 提取 loadLevel 与 resetRoundForCurrentLevel，减少重复的重置与摆位代码

\- 为第二关设置不同的 bird 数量，使两关不仅布局不同，难度也不同



\## Day 9

\- 修复连锁效果尚未结算完就提前判输的问题

\- 加入 sceneStillMoving 判断，延后失败结算

\- 修复第二关布局配置错误，保证两个敌人和三个障碍物正常出现

\- 统一第三个障碍物 block3 的耐久与重置逻辑

\- 修改第二关横梁掉落规则：需要左右两个支撑都被破坏后才会下落

\- 增强 block3 的关卡作用，使第二关不只是摆位变化而是规则变化



\## Day 10

\- 继续推进关卡系统重构，引入并接入 LevelData，统一管理关卡位置与规则数据

\- 将 loadLevel 改为返回 LevelData，并使用 currentLevelData 作为当前关卡数据来源

\- 将多个敌人的判断逻辑抽为函数，包括剩余敌人判断、全灭判断、命中检测和统一绘制

\- 将多个建筑的受击逻辑抽为 handleBirdHitBlock 函数，减少重复代码

\- 完成第二关 enemy3 的接入，包括初始化、命中、绘制与胜利条件

\- 完成第二关 block3 的接入，并使第二关横梁下落规则依赖左右两个支撑

\- 继续把主循环中的旧位置变量逻辑迁移到 currentLevelData

\- 当前版本已进入“可扩展重构”阶段，为后续加入多种小鸟、技能和更复杂建筑结构打基础



\## Day 11

\- 不再继续在旧 main.cpp 上补丁式修改，改为整体重组代码结构

\- 将代码重新整理为：基础类型定义、配置函数、重置函数、逻辑辅助函数、主循环五个部分

\- 引入 Enemy、Block、LevelData、RuntimeState 等结构体，使敌人、建筑、关卡数据和运行状态分离

\- 使用数组统一管理多个敌人和多个建筑，减少 enemy / enemy2 / enemy3、block / block2 / block3 的散乱写法

\- 将关卡配置集中到 loadLevel 中，统一由 LevelData 控制敌人、建筑、鸟数量和横梁支撑规则

\- 将整关重置逻辑集中到 resetLevel 中，统一处理开始游戏、切换关卡和重开

\- 将 bird 打建筑、bird 打敌人、敌人与建筑绘制等重复逻辑抽为函数

\- 保留原有核心玩法：两关、第三敌人、第三建筑、横梁下落、连锁击杀、N 切换下一只 bird、R 重开

\- 初步接入 BirdStats，使第二关最后一只 bird 切换为更高建筑伤害的重击鸟

