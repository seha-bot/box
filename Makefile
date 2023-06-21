FLAGS := -Iinc -Ibuild/deps
LIBS := -lm

all: build build/main

dep := $(addprefix build/,$(shell cat deps.list))
$(dep): deps.list
	@mkdir -p build/deps && cd build/deps && \
	GET=$(patsubst build/%,https://%,$@) && \
	echo Downloading $$GET && \
	curl -O $$GET
	@mkdir -p $$(dirname $@) && touch $@

build: | $(dep)
	@echo $(FLAGS) | tr " " "\n" > compile_flags.txt
	@mkdir -p build
	@echo -e "src := \$$(wildcard deps/*.c)\n\
	obj := \$$(src:deps/%.c=%.o)\n\
	all: \$$(obj)\n\
	%.o: deps/%.c\n\t\
	@gcc -c -Ideps \$$^ -o \$$@" > build/Makefile
	@cd build/ && make

DEPS := $(wildcard build/*.d)
-include $(DEPS)

build/%.o: src/%.c
	@gcc -g -c -MD $(FLAGS) $< -o $@

src := $(wildcard src/*.c)
obj := $(src:src/%.c=build/%.o)

build/main: $(obj)
	@gcc -g build/*.o -Llib $(LIBS) -o $@

clean:
	rm -rf build compile_flags.txt

