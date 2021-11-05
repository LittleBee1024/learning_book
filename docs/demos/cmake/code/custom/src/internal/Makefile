all: gen

# require python package "cog"
gen: clean
	python3 -m cogapp -d -s " //cog generated" -o animal.cpp animal.cpp.cog

clean:
	@rm -rf animal.cpp

.PHYON: all gen clean
