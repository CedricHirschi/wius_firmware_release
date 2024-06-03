SUBDIRS := $(wildcard */.)
OUTPUT_DIR := output

# remove output directory from subdirs
SUBDIRS := $(filter-out $(OUTPUT_DIR)/.,$(SUBDIRS))

ifndef VERBOSE
MAKEFLAGS += --no-print-directory --silent -j
endif

.PHONY: all $(SUBDIRS) compile clean new

all: $(SUBDIRS)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(SUBDIRS):
	if [ $@ != $(OUTPUT_DIR) ]; then \
		echo -e "Building \033[0;34m$@\033[0m"; \
		cp template.typ $@; \
		cp subdir.mak $@/Makefile; \
		$(MAKE) -C $@ all; \
	fi

compile: all $(OUTPUT_DIR)
	for dir in $(SUBDIRS); do \
		echo -e "Compiling \033[0;34m$$dir\033[0m"; \
		$(MAKE) -C $$dir compile; \
	done

clean:
	for dir in $(SUBDIRS); do \
		echo -e "Cleaning \033[0;34m$$dir\033[0m"; \
		$(MAKE) -C $$dir clean; \
		rm -rf $$dir/Makefile; \
		rm -rf $$dir/template.typ; \
	done
	echo -e "Cleaning \033[0;34m$(OUTPUT_DIR)\033[0m"; \
	rm -rf output
	rm -rf *.pdf

new:
	mkdir -p $(NAME)
	mkdir -p $(NAME)/assets
	echo "title: \"$(NAME)\"" > $(NAME)/metadata.yaml
	echo "template: \"template.typ\"" >> $(NAME)/metadata.yaml
	echo "# $(NAME)" > $(NAME)/$(NAME).md
	echo "" >> $(NAME)/$(NAME).md
	echo "Created new directory \`$(NAME)\` with \`$(NAME).md\` and \`metadata.yaml\`" >> $(NAME)/$(NAME).md
	echo "" >> $(NAME)/$(NAME).md
	echo "## done" >> $(NAME)/$(NAME).md