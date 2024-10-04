# Commander - A Header-only C++ Command Management Library

**Commander** is a lightweight, header-only library designed to simplify command parsing, context handling, and command execution for C++ projects. The library provides a flexible and extensible interface for defining commands with custom formats, argument parsing, and execution logic, making it ideal for scenarios like scripting systems, console commands, and user input processing.

### Key Features:
- **Interface-based Command System**: Easily define commands by implementing the `InterfaceCommand` class, allowing customization of parsing, context management, and execution.
- **Template-based Command Implementation**: Use the `ImplCommand` template to create commands with automatic argument parsing based on a format string, leveraging powerful tools like `fmt` and `scn` for formatting and scanning inputs.
- **Context Management**: Commands can store and utilize a custom context, allowing them to carry and operate on necessary runtime data.
- **Command Registration and Execution**: The `ExecuterCommands` class allows dynamic registration and management of commands using a map, enabling easy lookup and execution of commands by name.
- **Regular Expression-based Command Extraction**: Extract command names from format strings using `ctre`, allowing for flexible and customizable command formats.
- **Integration with Popular Libraries**: Leverages well-known libraries such as `fmt`, `scn`, `ctre`, and `plog` for powerful logging, formatting, and parsing functionalities.

### Example Usage:
Define a new command by inheriting from `ImplCommand` and providing a format string for argument parsing. The library supports automatic scanning of arguments based on the provided format and effortless execution of commands.

```cpp
struct MyCommand : ox::ImplCommand<ox::InterfaceCommand<std::string>, "{0}", std::string> {
    int execute() override {
        fmt::print("Executed command with argument: {}\n", std::get<0>(args()));
        return 0;
    }
};
```

Commands can be registered and executed using the `ExecuterCommands` system:

```cpp
ox::ExecuterCommands<std::string> commandManager;
commandManager.create_command<MyCommand>();

if (auto cmd = commandManager["MyCommand"]; cmd) {
    cmd->parser("Hello");
    cmd->execute();
}
```

### Dependencies:
- `fmt`: Formatting and printing
- `scn`: Input scanning and argument parsing
- `ctre`: Regular expressions for command extraction
- `plog`: Logging utilities
- `fixed_string`: Compile-time fixed string support for format strings

This library is well-suited for applications where command execution is crucial, such as game engines, scripting systems, bots, or CLIs.