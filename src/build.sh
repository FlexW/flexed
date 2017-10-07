#!/bin/bash
g++ flexed_text_buffer.cpp keyboard_map.cpp keyboard_handler.cpp flexed_text_buffer_container.cpp flexed_global_text_buffer_container.cpp flexed_editor.cpp main.cpp -o editor -lboost_filesystem -lboost_system `pkg-config gtkmm-3.0 --cflags --libs` `pkg-config --cflags --libs gtksourceviewmm-3.0`
