BIN_DIR := ../../bin/gcc
OBJ_DIR := ../../obj/gcc
SRC_DIR := ../../src
FINAL_OUTPUT := $(BIN_DIR)/sxTest
ifdef WINDIR
FINAL_OUTPUT := $(FINAL_OUTPUT).exe
endif

ANY_C_CPP = \
	$(wildcard $(SRC_DIR)$(1)/*.c) \
	$(wildcard $(SRC_DIR)$(1)/*.cpp)
SOURCES := $(call ANY_C_CPP)
SOURCES += $(call ANY_C_CPP,/helper)
SOURCES += $(call ANY_C_CPP,/MaiSoft)
SOURCES += $(call ANY_C_CPP,/test)

C_SOURCES := $(filter %.c, $(SOURCES))
CPP_SOURCES := $(filter %.cpp, $(SOURCES))
SRC_DEPENDS := $(foreach ITEM, $(subst $(SRC_DIR)/,,$(SOURCES)), depend/$(ITEM).d)

C_OBJECTS := $(patsubst %.c, %.c.o, $(C_SOURCES))
C_OBJECTS := $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(C_OBJECTS))
CPP_OBJECTS := $(patsubst %.cpp, %.cpp.o, $(CPP_SOURCES))
CPP_OBJECTS := $(patsubst $(SRC_DIR)%, $(OBJ_DIR)%, $(CPP_OBJECTS))

SRC_PATH_AFTER_SRC_DIR = $(subst $(SRC_DIR)/,,$<)
define COMPILE =
	@echo Compile: $(SRC_PATH_AFTER_SRC_DIR)
	@mkdir -p $(dir $@)
	@mkdir -p depend/$(dir $(SRC_PATH_AFTER_SRC_DIR))
	@gcc $1 -c '$<' -o'$@'
	@gcc $1 -MM '$<' -MT '$@' > depend/$(SRC_PATH_AFTER_SRC_DIR).d
endef

COMMON_FLAGS_0 := -O2 -I'$(SRC_DIR)'
C_FLAGS_0 := $(COMMON_FLAGS_0) -std=c99 -pedantic
CPP_FLAGS_0 := $(COMMON_FLAGS_0) -std=c++11
LINK_LIBS := -lstdc++

$(FINAL_OUTPUT): $(C_OBJECTS) $(CPP_OBJECTS)
	@echo Link as: $(abspath $(FINAL_OUTPUT))
	@mkdir -p $(BIN_DIR)
	@gcc -s -o'$@' $(C_OBJECTS) $(CPP_OBJECTS) $(LINK_LIBS)

-include $(SRC_DEPENDS)

$(OBJ_DIR)/%.c.o: $(SRC_DIR)/%.c
	$(call COMPILE, $(C_FLAGS_0))

$(OBJ_DIR)/helper/%.c.o: $(SRC_DIR)/helper/%.c
	$(call COMPILE, $(C_FLAGS_0))

$(OBJ_DIR)/MaiSoft/%.c.o: $(SRC_DIR)/MaiSoft/%.c
	$(call COMPILE, $(C_FLAGS_0))

$(OBJ_DIR)/test/%.c.o: $(SRC_DIR)/test/%.c
	$(call COMPILE, $(C_FLAGS_0))

$(OBJ_DIR)/test/%.cpp.o: $(SRC_DIR)/test/%.cpp
	$(call COMPILE, $(CPP_FLAGS_0))


all: $(FINAL_OUTPUT)

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/* $(FINAL_OUTPUT)

