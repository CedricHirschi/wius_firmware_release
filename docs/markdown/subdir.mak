DIRNAME = $(shell basename $(CURDIR))


BUILD_DIR = build
OUTPUT_DIR = ../output

MAIN_FILE = $(BUILD_DIR)/$(DIRNAME).typ
PDF_FILE = $(OUTPUT_DIR)/$(DIRNAME).pdf
MD_FILES = $(wildcard *.md)
TYPST_FILES = $(patsubst %.md,$(BUILD_DIR)/%.typ,$(md_files))

all: $(BUILD_DIR) $(MAIN_FILE)
compile: $(PDF_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	cp -r assets $(BUILD_DIR)
	cp template.typ $(BUILD_DIR)

$(BUILD_DIR)/%.typ: %.md 
	pandoc -s -t typst $< -o $@ --metadata-file=metadata.yaml

$(OUTPUT_DIR)/%.pdf: $(BUILD_DIR)/%.typ
	typst compile $< $@

$(MAIN): $(TYPST_FILES)
	pandoc -s -t typst --metadata-file=metadata.yaml $(md_files) -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf *.pdf

.PHONY: all clean compile 