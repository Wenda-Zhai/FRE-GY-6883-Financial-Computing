CXX = g++ #complier is g++
CXXFLAGS = -std=c++17 -Wall #use c++ codes, turn on warning
LDFLAGS = -lcurl #link to lcurl library


CXXFLAGS += -pthread  #turn on pthreading in complier
LDFLAGS += -pthread #turn on pthreading in link

EXE_NAME = HW14 #executable file name
TARGET = $(EXE_NAME) #set TARGET as EXE_NAME

SOURCES = $(wildcard *.cpp) #使用 wildcard 函数获取当前目录下所有以 .cpp 结尾的文件，赋值给变量 SOURCES。
OBJECTS = $(SOURCES:%.cpp=%.o) #将 SOURCES 中的每个 .cpp 文件名替换为对应的 .o 文件，生成变量 OBJECTS。

HEADERS = $(wildcard *.h) #使用 wildcard 函数获取当前目录下所有以 .h 结尾的文件，赋值给变量 HEADERS.

all: $(TARGET) #默认目标规则，编译并链接所有内容，生成目标可执行文件。

# 定义如何生成可执行文件：
# 依赖于所有目标文件 $(OBJECTS)。
# 执行命令：使用 $(CXX) 将所有目标文件链接成可执行文件 $(TARGET)，使用链接选项 $(LDFLAGS)。

# 打印一条信息，表示正在链接目标文件。
#实际执行的命令：用 g++ 链接目标文件生成可执行文件。
# 打印一条成功消息，表示构建完成。

$(TARGET): $(OBJECTS) 
	@echo "[INFO] Linking object files to create the executable..."
	@$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "[SUCCESS] Build complete! Executable created at: $(TARGET)"

# 定义如何从 .cpp 文件生成对应的 .o 文件：
# 依赖于 .cpp 文件和所有头文件 $(HEADERS)。
# 打印一条信息，表示正在编译源文件 $< 到目标文件 $@。
# 实际执行的命令：使用 $(CXX) 编译 $<（当前的 .cpp 文件）生成对应的目标文件 $@。

%.o: %.cpp $(HEADERS)
	@echo "[INFO] Compiling $< -> $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# 定义清理规则，用于删除生成的文件。
# 打印一条信息，表示正在清理。
# 删除所有目标文件和可执行文件。
# 打印一条成功消息，表示清理完成。

clean:
	@echo "[INFO] Cleaning up build files..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "[SUCCESS] Clean complete!"

# 定义运行规则，依赖于 all（确保目标文件已生成）。
# 打印一条信息，表示即将运行程序。
# 运行生成的可执行文件。

run: all
	@echo "[INFO] Running the program..."
	@./$(TARGET)

.PHONY: all clean run
