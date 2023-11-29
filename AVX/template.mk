
TOP_DIR := .
SRC_DIR := $(TOP_DIR)
COMMON_DIR := $(TOP_DIR)/../../common
OBJ_DIR := $(TOP_DIR)/obj
BIN_DIR := $(TOP_DIR)/bin

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(SRCS:%=$(OBJ_DIR)/%.o)
TARGET := $(BIN_DIR)/a.out

CXX := g++
CXXFLAGS := -g -O1 -std=c++20 -mavx2 -Wall -MMD -MP
CPPFLAGS := -I$(COMMON_DIR)
LDFLAGS := $(CPPFLAGS)

RM := rm -rf
MKDIR := mkdir -p


.PHONY: run clean $(OBJ_DIR) $(BIN_DIR)

all: run

run:  $(TARGET)
	$(TARGET)

$(TARGET): $(BIN_DIR) $(OBJ_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(CXXFLAGS)

clean:
	-@$(RM) $(BIN_DIR) $(OBJ_DIR)

$(OBJ_DIR)/%.cpp.o: %.cpp
	-@$(MKDIR) $(dir $@)
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(LDFLAGS)

$(OBJ_DIR):
	-@$(MKDIR) $(OBJ_DIR)

$(BIN_DIR):
	-@$(MKDIR) $(BIN_DIR)

