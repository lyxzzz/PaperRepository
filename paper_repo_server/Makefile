OBJ_DIR=objs
CFILE_DIR=src
CC=g++

CPPFILES=$(wildcard $(CFILE_DIR)/*.cpp)
OBJS=$(patsubst $(CFILE_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPPFILES))
DEPS=$(patsubst $(CFILE_DIR)/%.cpp,$(OBJ_DIR)/%.d,$(CPPFILES))
MY_APP_FLAGS=-g -Iinclude -std=c++11

target=paper_repository

.PHONY: all clean cleanobj

all:$(target)

-include $(DEPS)

$(OBJ_DIR)/%.d: $(CFILE_DIR)/%.cpp
	$(CC) -MM $(MY_APP_FLAGS) $< | sed 's/^../objs\/&/g' > $@

$(OBJ_DIR)/%.o: $(CFILE_DIR)/%.cpp $(OBJ_DIR)/%.d
	$(CC) -c $(MY_APP_FLAGS) $< -o $@

$(target):$(OBJS) $(LIB_FILES)
	$(CC) $(MY_APP_FLAGS) $(OBJS) $(LD_FLAGS) -o $@
	mkdir repository

clean:
	$(RM) $(OBJS) $(DEPS)
	$(RM) $(target)
	$(RM) -rf repository

cleanobj:
	$(RM) objs/*