/*# MIT License
#
# Copyright (c) 2020 Ferhat Geçdoğan All Rights Reserved.
# Distributed under the terms of the MIT License.
#*/

#ifndef INTELLIGEN_TUI_HPP
#define INTELLIGEN_TUI_HPP

#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

/* TODO:
	* Colorizing support for boxes, labels (use colorized)
	* Layer & fully tag support.
	* ffont-like label support with resize option.
*/

/* intelligenTUI - really simple, single header TUI library for C++ (work-in-progress)
   ----
   structures:
   
   pos_t : position of label_t | box_t 
   tag_t : tag name & number (layer) of label_t | box_t
   ui_t  : pos_t & tag_t with child option
   
   ---
   label_t : label widget (ui_t & string)
   box_t   : box widget   (ui_t & width char & height char)
   log_t   : initialize checker (bool)

   ---
   initialize:
   
    - initialize a box:
        * box_t box = box_init(ui_init(pos_init(x, y, w, h), tag_init("my-box", layer_number)),
		    width_char, height_char);
   	
   	- initialize a label:
        *  label_t label = label_init(ui_init(pos_init(x, y, 0, 0), tag_init("my-label", layer_number)),
		"Hello, world!");
		
			? w & h must be 0 (h : terminal emulator's size) 
   		
   	- display a box:
   	    * create_box(box, child);
   		    ? child = false | true
	
	- display a label as child:
	    * create_lchild(box, label);

	- display a box as child:
	    * create_child(box, child);

	----
	
*/

/* standard termios structure */
struct termios ui_stream;

typedef struct {
	unsigned x;
	unsigned y;
	
	unsigned w;
	unsigned h;
} pos_t;

typedef struct {
	std::string tag_name;
	unsigned tag_number;
} tag_t;

typedef struct {
	pos_t pos;
	tag_t tag;
	bool child = false;
} ui_t;

typedef struct {
	ui_t ui;
	std::string label;
} label_t;

typedef struct {
	ui_t ui;
	
	const char* w_ch = "-";
	const char* h_ch = "|";
} box_t;

typedef struct {
	bool initialized = false;
} log_t;


/* intelligenTUI namespace */ 
namespace itui {
	/* Initialize position */
	pos_t pos_init(unsigned x, unsigned y, unsigned w, unsigned h) {
		pos_t init;
	
		init.x = x;
		init.y = y;
	
		init.w = w;
		init.h = h;
	
		return init;
	}

	/* Initialize tag */
	tag_t tag_init(std::string tag_name, unsigned tag_number) {
		tag_t tag;
		
		tag.tag_name   = tag_name;
		tag.tag_number = tag_number;
		
		return tag;
	}
	
	/* Initialize UI (position, tag) */
	ui_t ui_init(pos_t pos, tag_t tag) {
		ui_t ui;
	
		ui.pos = pos;
		ui.tag = tag;

		return ui;
	}
	
	/* Initialize label */
	label_t label_init(ui_t ui, std::string _label) {
		label_t label;
	
		label.ui = ui;

		label.label = _label;
	
		return label;
	}
	
	/* Initialize box */
	box_t box_init(ui_t ui, const char* w_ch, const char* h_ch) {
		box_t box;
		
		box.ui = ui;
	
		box.w_ch = w_ch;
		box.h_ch = h_ch;

		return box;
	}

	/* Move cursor to x, y */
	void move_pos(unsigned x, unsigned y) {
		std::cout << "\033[" << x << ";" << y << "H";
	}

	log_t move_pos(pos_t pos) {
		log_t log;
		
		std::cout << "\033[" << pos.x << ";" << pos.y << "H";
	
		log.initialized = true;
		
		return log;
	}

	/* Create label */
	void create_label(label_t label, bool child) {
		move_pos(label.ui.pos);
	
		std::cout << label.label;
	}	

	/* Create box with child control */
	log_t create_box(box_t box, bool child) {
		int x, y;
	
		x = box.ui.pos.x;
		y = box.ui.pos.y;
	
		log_t log;
	
		if(move_pos(box.ui.pos).initialized != false) {
			for(unsigned i = 0; i < box.ui.pos.w; i++) {
				std::cout << box.w_ch;
			}
	
			std::cout << "\n";

			
			for(unsigned q = 0; q < box.ui.pos.h; q++) {
				if(child == true) {
					for(unsigned i = 1; i < box.ui.pos.y; ++i) {
						std::cout << " ";
					}
				}
			
				std::cout << box.h_ch;
		
				for(unsigned i = 1; i < box.ui.pos.w; ++i) {
					std::cout << " ";
				}
			
				std::cout << box.h_ch << "\n";
			}
	
			if(child == true) {
				for(unsigned i = 1; i < box.ui.pos.y; ++i) {
					std::cout << " ";
				}
			}
			
			for(unsigned i = 0; i < box.ui.pos.w; i++) {
				std::cout << box.w_ch;
			}
	
			log.initialized = true;
		} else {
			log.initialized = false;
		}

		return log;
	}

	/* Create child */
	log_t create_child(box_t main, box_t child) {
		log_t log;
	
		if(child.ui.pos.w && child.ui.pos.w < main.ui.pos.w && main.ui.pos.h) {
			std::cout << "\033[1;96m";
			create_box(child, true);
			log.initialized = true;
		} else {
			log.initialized = false;
		}
	
		return log;
	}
	
	/* Create label-child */
	log_t create_lchild(box_t main, label_t lchild) {
		log_t log;
	
		std::cout << "\033[1;91m";
		create_label(lchild, true);
		log.initialized = true;
	
		return log;
	}

	
	/* Disable raw mode */
	void disable_raw_mode() {
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &ui_stream);
	}
	
	void enable_raw_mode() {
		tcgetattr(STDIN_FILENO, &ui_stream);
	  	atexit(disable_raw_mode);

	  	struct termios raw = ui_stream;
	  	raw.c_lflag &= ~(ECHO | ICANON);

	  	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	}
	
	void refresh_buffer() {
		std::cout << "\x1b[2J";
	}
	
	void clear_buffer() {
		std::cout << "\x1b[2J";
		std::cout << "\x1b[H";
	}
}

#endif // INTELLIGEN_TUI_HPP
