OBJ_DIR=objs
CFILE_DIR=src
CC=g++

CPPFILES=$(wildcard $(CFILE_DIR)/*.cpp)
OBJS=$(patsubst $(CFILE_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPPFILES))
DEPS=$(patsubst $(CFILE_DIR)/%.cpp,$(OBJ_DIR)/%.d,$(CPPFILES))
MY_APP_FLAGS=-g -Iinclude -static
MY_LD_FLAGS=-lws2_32

target=paper_repository

.PHONY: all clean cleanobj

all:$(target)

-include $(DEPS)

$(OBJ_DIR)/%.d: $(CFILE_DIR)/%.cpp
	$(CC) -MM $(MY_APP_FLAGS) $<

$(OBJ_DIR)/%.o: $(CFILE_DIR)/%.cpp $(OBJ_DIR)/%.d
	$(CC) -c $(MY_APP_FLAGS) $< -o $@

$(target):$(OBJS) $(LIB_FILES)
	$(CC) $(MY_APP_FLAGS) $(OBJS) $(MY_LD_FLAGS) -o $@

clean:
	$(RM) $(OBJS) $(DEPS)
	$(RM) $(target)

cleanobj:
	$(RM) objs/*