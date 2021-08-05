src = README
proj = EPGPurge

html = $(addsuffix .html,$(src))
extdir="/home/mark/Data/src/asciidoctor-extensions-lab/lib"
site = bullet:/usr/share/nginx/markb/taps/$(proj)/

all: $(html)
html: $(html)

# Table of contents level
TOC = 3

tmp = .tmp.html

%.html: %.adoc Makefile
	asciidoctor -a icons=font -a data-uri -a numbered \
		-r $(extdir)/google-analytics-docinfoprocessor.rb \
		-a google-analytics-account=UA-23551033-2 \
		-a toclevels=$(TOC) \
		-r asciidoctor-diagram \
		-o $(tmp) \
		$<
	add-html-meta-tag robots noindex $(tmp) >$(html)
	@rm -f *.png diag-*.cache $(tmp)

deploy:
	rsync -avc $(html) $(site)

serve:
	live-server-command $(src).adoc make $(html)

clean:
	rm -f $(html) *.xml *.png *.cache $(tmp)

# vim: se ts=4:
