BIN_DIR := ../../bin/gcc
OBJ_DIR := ../../obj/gcc
SRC_DIR := ../../src
FINAL_OUTPUT := $(BIN_DIR)/sxTest.exe

ANY_C_CPP = \
	$(wildcard $(SRC_DIR)$(1)/*.c) \
	$(wildcard $(SRC_DIR)$(1)/*.cpp)
SOURCES := $(call ANY_C_CPP)
SOURCES += $(call ANY_C_CPP,/helper)
SOURCES += $(call ANY_C_CPP,/MaiSoft)
SOURCES += $(call ANY_C_CPP,/test)

C_SOURCES := $(filter %.c, $(SOURCES))
CPP_SOURCES := $(filter %.cpp, $(SOURCES))
SOURCE_DEPENDS := $(foreach ITEM, $(subst $(SRC_DIR)/,,$(SOURCES)), depend/$(ITEM).d)

C_OBJECTS := $(patsubst %.c, %.c.o, $(C_SOURCES))
C_OBJECTS := $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(C_OBJECTS))
CPP_OBJECTS := $(patsubst %.cpp, %.cpp.o, $(CPP_SOURCES))
CPP_OBJECTS := $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(CPP_OBJECTS))

CC_FLAG1 := -O2 -I'$(SRC_DIR)'
define COMPILE1 =
	mkdir -p $(dir $@)
	mkdir -p depend/$(dir $(subst $(SRC_DIR)/,,$<))
	gcc $(CC_FLAG1) -c '$<' -o'$@'
	gcc $(CC_FLAG1) -MM '$<' -MT '$@' > depend/$(subst $(SRC_DIR)/,,$<).d
endef

$(FINAL_OUTPUT): $(C_OBJECTS) $(CPP_OBJECTS)
	mkdir -p $(BIN_DIR)
	gcc -s -o'$@' $(C_OBJECTS) $(CPP_OBJECTS) -lstdc++
	@echo 
	@echo MAKE succeeded! See:
	@echo $(abspath $(FINAL_OUTPUT))

-include $(SOURCE_DEPENDS)

$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	$(COMPILE1)

$(OBJ_DIR)/helper/%.c.o: $(SRC_DIR)/helper/%.c
	$(COMPILE1)

$(OBJ_DIR)/MaiSoft/%.c.o: $(SRC_DIR)/MaiSoft/%.c
	$(COMPILE1)

$(OBJ_DIR)/test/%.c.o: $(SRC_DIR)/test/%.c
	$(COMPILE1)

$(OBJ_DIR)/test/%.cpp.o: $(SRC_DIR)/test/%.cpp
	$(COMPILE1)
