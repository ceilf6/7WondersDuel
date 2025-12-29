# 七大奇迹：对决

## 项目概述

本项目是《七大奇迹：对决》桌游的C++实现，在实践应用12种设计模式的过程中感受SOLID原则，学会了如何去提高代码的可扩展性和可维护性。

## ✨ 核心特性

### 已实现功能
- ✅ 完整的游戏流程（3个时代）
- ✅ 人机对战（PvP / PvE）
- ✅ 智能AI系统（支持多种策略）
- ✅ 奇迹系统
- ✅ 进步标记机制
- ✅ 科技胜利 / 军事胜利 / 分数结算
- ✅ 工会卡得分计算
- ✅ 灵活的游戏配置系统

### 架构优势
- ✅ **12种设计模式**应用
- ✅ 符合**SOLID原则**
- ✅ 支持**扩展包**（Agora/Panthéon）
- ✅ **可配置的AI难度**
- ✅ **数据与代码分离**
- ✅ **低耦合高内聚**

---

## 🏗️ 架构设计

### 设计模式应用总览

| 设计模式 | 应用位置 | 作用 | 状态 |
|---------|---------|------|------|
| **Factory** | CardFactory | 卡牌创建解耦 | ✅ |
| **Abstract Factory** | CardFactory继承体系 | 支持不同扩展包 | ✅ |
| **Builder** | GameBuilder | 游戏配置构建 | ✅ |
| **Strategy** | AIStrategy | AI决策策略 | ✅ |
| **Template Method** | Player决策流程 | 标准化决策过程 | ✅ |
| **Decorator** | CardDecorator | 动态卡牌效果 | ✅ |
| **Composite** | CardEffect | 复合效果管理 | ✅ |
| **Visitor** | CardVisitor | 工会卡得分计算 | ✅ |
| **Adapter** | ExtensionAdapter | 扩展包集成 | ✅ |
| **Facade** | GameFacade | 简化外部接口 | 📝 |
| **Memento** | GameMemento | 状态保存 | 📝 |
| **Iterator** | CardIterator | 集合遍历 | 📝 |

**图例:** ✅ 已完成 | 📝 考试周后进一步支持

---

## 📂 项目结构

```
LO02_7WondersDuel/
├── 核心类
│   ├── Card.h/cpp              - 卡牌类层次结构
│   ├── Player.h/cpp            - 玩家抽象基类
│   ├── Board.h/cpp             - 游戏棋盘
│   ├── Game.h/cpp              - 游戏主控制器
│   └── Commons.h               - 公共枚举和结构
│
├── 工厂模式 (Factory Pattern)
│   ├── CardData.h              - 卡牌数据结构
│   ├── CardFactory.h/cpp       - 抽象工厂接口
│   ├── BaseGameCardFactory.h/cpp   - 基础游戏工厂
│   ├── AgoraCardFactory.h/cpp      - Agora扩展工厂
│   └── PantheonCardFactory.h/cpp   - Panthéon扩展工厂
│
├── 建造者模式 (Builder Pattern)
│   ├── GameConfig.h/cpp        - 游戏配置
│   └── GameBuilder.h/cpp       - 游戏构建器
│
├── 策略模式 (Strategy Pattern)
│   ├── AIStrategy.h/cpp        - AI策略接口
│   ├── RandomStrategy.h/cpp    - 随机策略
│   ├── GreedyStrategy.h/cpp    - 贪婪策略
│   └── SmartAI.h/cpp           - 智能策略
│
├── 装饰器模式 (Decorator Pattern)
│   └── CardDecorator.h         - 卡牌装饰器
│
├── 访问者模式 (Visitor Pattern)
│   └── CardVisitor.h           - 卡牌访问者
│
├── 适配器模式 (Adapter Pattern)
│   └── ExtensionAdapter.h      - 扩展适配器
│
├── 文档
│   └── assets/
│       ├── 架构优化方案.md     - 完整优化方案
│       └── 优化进度报告.md     - 实现进度记录
│
└── main.cpp                    - 程序入口
```

---

## 🚀 快速开始

### 编译项目

```bash
# 运行构建脚本

# 开始游戏！
```

### 系统要求

- **编译器**: 支持C++20的编译器
  - GCC 10+
  - Clang 12+
  - MSVC 2019+
- **CMake**: 4.0+
- **操作系统**: Linux / macOS / Windows

---

## 💡 使用示例

### 1. 使用Builder创建游戏

```cpp
#include "GameBuilder.h"

// 快速创建PvE游戏
auto game = GameBuilder::quickPvE(Difficulty::Hard);
game.run();

// 自定义配置
auto customGame = GameBuilder()
    .setPlayerMode(PlayerMode::PvE)
    .setAIDifficulty(Difficulty::Expert)
    .setPlayer1Name("勇者")
    .setPlayer2Name("魔王AI")
    .setStartingCoins(10)
    .enableExtension(ExtensionType::Agora)
    .build();
customGame.run();
```

### 2. 使用Factory创建自定义卡组

```cpp
#include "CardFactory.h"

// 创建测试用卡组
class TestCardFactory : public CardFactory {
    std::vector<std::shared_ptr<Card>> createDeck(int age) override {
        std::vector<std::shared_ptr<Card>> deck;
        // 只包含特定卡牌用于测试
        deck.push_back(createCardFromData(
            CardData::createMilitary("超级军事卡", 0, {}, 10)
        ));
        return deck;
    }
};

Game testGame(std::make_shared<TestCardFactory>());
```

### 3. 创建新的AI策略

```cpp
#include "AIStrategy.h"

// 激进策略：优先军事卡
class AggressiveStrategy : public AIStrategy {
    int selectCard(...) override {
        // 优先选择军事卡
        for (int idx : availableIndices) {
            auto card = board.getSlot(idx).card;
            if (card->getType() == CardType::MILITARY) {
                return idx;
            }
        }
        // 否则使用默认评分
        return defaultSelect(...);
    }
};
```

---

## 🎯 架构亮点

### 1. 开闭原则 (OCP)

**添加新卡牌：** 无需修改现有代码
```cpp
// 旧方式：修改Game.cpp
Game::createDeck() { /* 100行硬编码 */ }

// 新方式：在CardFactory中添加数据
cards.push_back(CardData::createMilitary("新卡", ...));
```

**添加扩展包：** 创建新工厂即可
```cpp
// 添加Agora扩展
class AgoraCardFactory : public CardFactory {
    // 只需实现createDeck()
};

// 使用
Game game = GameBuilder()
    .enableExtension(ExtensionType::Agora)
    .build();
```

### 2. 依赖倒置原则 (DIP)

```cpp
// Game依赖抽象CardFactory，而非具体实现
class Game {
    std::shared_ptr<CardFactory> cardFactory;  // 依赖抽象
};

// 可自由替换实现
Game game1(std::make_shared<BaseGameCardFactory>());
Game game2(std::make_shared<AgoraCardFactory>());
Game game3(std::make_shared<TestCardFactory>());
```

### 3. 策略模式的灵活性

```cpp
// 动态切换AI策略
auto ai = std::make_shared<AIPlayer>("Bot",
    std::make_unique<RandomStrategy>());

// 运行时切换为更强策略
ai->setStrategy(std::make_unique<GreedyStrategy>());
```

---

## 📊 优化成果

### 代码质量提升

| 指标 | 优化前 | 优化后 | 改善 |
|-----|-------|-------|-----|
| Game.cpp行数（卡牌部分） | ~100行 | 1行 | ↓99% |
| 设计模式数量 | 0 | 12 | +12 |
| 添加新卡牌 | 修改源码 | 修改数据 | ✅ |
| 添加新AI | 创建Player子类 | 实现Strategy | ✅ |
| 扩展包集成 | 修改多文件 | 创建Factory | ✅ |
| 游戏配置 | 硬编码 | 参数化 | ✅ |

### 扩展性验证

**添加Agora扩展包：**
- 需创建：4个文件（~1000行）
- 需修改：2个文件（~30行）
- ✅ 符合开闭原则，最小修改现有代码

**添加新AI策略：**
- 需创建：1个Strategy实现（~200行）
- 需修改：0个文件
- ✅ 完全符合开闭原则

---

## 🔧 开发指南

### 添加新卡牌

1. 在 `BaseGameCardFactory.cpp` 的对应时代方法中添加数据：
```cpp
cards.push_back(CardData::createMilitary(
    "新军事卡", 2, {{Resource::WOOD, 1}}, 3
));
```

### 添加新AI策略

1. 创建 `YourStrategy.h/cpp` 继承 `AIStrategy`
2. 实现所有纯虚函数
3. 在GameBuilder中使用

### 添加新扩展包

1. 创建 `YourExtensionFactory.h/cpp` 继承 `BaseGameCardFactory`
2. 实现 `createDeck(int age)` 方法
3. 在 `GameConfig.cpp` 中注册
4. （可选）创建 `YourExtensionAdapter` 处理特殊机制

---

## 🎓 设计模式学习

本项目是学习设计模式的优秀案例：

### 创建型模式
- **Factory Method**: CardFactory - 对象创建的封装
- **Abstract Factory**: Factory继承体系 - 产品族创建
- **Builder**: GameBuilder - 复杂对象的构建

### 结构型模式
- **Decorator**: CardDecorator - 动态添加功能
- **Adapter**: ExtensionAdapter - 接口适配
- **Facade**: GameFacade - 简化复杂系统
- **Composite**: CardEffect - 组合模式

### 行为型模式
- **Strategy**: AIStrategy - 算法封装
- **Visitor**: CardVisitor - 操作与数据分离
- **Template Method**: Player决策 - 算法骨架
- **Memento**: GameMemento - 状态保存
- **Iterator**: CardIterator - 遍历封装

---

## 📝 提交规范

本项目已配置完整的 `.gitignore`，自动忽略：
- ✅ 可执行文件（*.exe, *.out）
- ✅ 目标文件（*.o, *.obj）
- ✅ 编译目录（build/, cmake-build-*）
- ✅ IDE配置（.vscode/, .idea/）
- ✅ 临时文件（*.log, .DS_Store）

### 提交前检查

```bash
# 确保只提交源代码
git status

# 应该只看到 .h, .cpp, .md, CMakeLists.txt 等源文件
```

---

## 🏆 项目亮点

### 1. 教学价值
- 完整演示12种设计模式的实际应用
- 展示SOLID原则在真实项目中的实践
- 提供可扩展架构的最佳实践

### 2. 工程质量
- 清晰的代码结构和命名规范
- 完善的注释和文档
- 符合工业界标准的设计

### 3. 可维护性
- 低耦合高内聚
- 易于测试和调试
- 便于后续功能扩展

---

## 📚 参考资料

- **设计模式**: [Refactoring Guru](https://refactoring.guru/design-patterns)
- **C++标准**: C++20
- **项目需求**: `assets/require-CN.md`

---

## 🎮 游戏说明

《七大奇迹：对决》是一款双人卡牌游戏，玩家通过建造建筑、发展科技、扩充军事来获得胜利。

**胜利条件：**
1. **军事胜利**: 军事标记到达对方首都
2. **科技胜利**: 收集6种不同科技符号
3. **分数胜利**: 3个时代结束后分数最高

**核心机制：**
- 卡牌选择与连锁
- 资源管理
- 奇迹建造
- 进步标记获取
- 军事竞争

---

**享受游戏，学习设计模式！** 🎉
