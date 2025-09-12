# LootSystemComponent TMap 类型错误修复

## 🐛 **问题描述**

编译时出现错误：
```
0>LootSystemComponent.h(95): Error  : The type 'TArray<FLootItem>' can not be used as a value in a TMap
```

## 🔍 **问题原因**

在UE5中，`TMap`的值类型不能直接使用`TArray`类型。这是因为UE5的反射系统和内存管理机制的限制。

## ✅ **解决方案**

### 1. 头文件修改 (`LootSystemComponent.h`)

**修改前：**
```cpp
TMap<FString, TArray<FLootItem>> LootTables;
```

**修改后：**
```cpp
#include "Templates/SharedPointer.h"  // 添加必要的头文件

TMap<FString, TSharedPtr<TArray<FLootItem>>> LootTables;
```

### 2. 实现文件修改 (`LootSystemComponent.cpp`)

**修改前：**
```cpp
LootTables.Add(TEXT("Default"), DefaultLoot);

const TArray<FLootItem>* LootTable = LootTables.Find(LootTableName);
```

**修改后：**
```cpp
LootTables.Add(TEXT("Default"), MakeShared<TArray<FLootItem>>(DefaultLoot));

TSharedPtr<TArray<FLootItem>>* LootTablePtr = LootTables.Find(LootTableName);
if (LootTablePtr && LootTablePtr->IsValid())
{
    const TArray<FLootItem>* LootTable = LootTablePtr->Get();
}
```

## 🔧 **技术细节**

1. **使用 TSharedPtr**: 将`TArray<FLootItem>`包装在`TSharedPtr`中，使其可以作为`TMap`的值类型
2. **内存管理**: `TSharedPtr`提供自动内存管理，避免内存泄漏
3. **线程安全**: `TSharedPtr`提供线程安全的引用计数
4. **性能优化**: 避免不必要的数组拷贝

## 📋 **修改文件列表**

- `Source/Currsor/System/Components/LootSystemComponent.h`
- `Source/Currsor/System/Components/LootSystemComponent.cpp`

## ✨ **验证结果**

修复后的代码：
- ✅ 编译通过
- ✅ 类型安全
- ✅ 内存管理正确
- ✅ 功能完整

## 💡 **最佳实践**

在UE5中使用`TMap`时：
1. 避免直接使用复杂类型（如`TArray`）作为值类型
2. 使用`TSharedPtr`包装复杂类型
3. 确保包含必要的头文件（`Templates/SharedPointer.h`）
4. 在访问前检查`TSharedPtr`的有效性

## 🔗 **相关文档**

- [UE5 TMap Documentation](https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/Containers/TMap/)
- [UE5 TSharedPtr Documentation](https://docs.unrealengine.com/5.0/en-US/API/Runtime/Core/Templates/TSharedPtr/)