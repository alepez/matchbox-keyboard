#ifndef HAVE_MB_KEYBOARD_H
#define HAVE_MB_KEYBOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/keysym.h>

#include <fakekey/fakekey.h>

#define WANT_DEBUG 1

#if (WANT_DEBUG)
#define DBG(x, a...) \
 fprintf (stderr,  __FILE__ ":%d,%s() " x "\n", __LINE__, __func__, ##a)
#else
#define DBG(x, a...) do {} while (0)
#endif

#define MARK() DBG("mark")

typedef void*  pointer ;

typedef struct List List;

struct List 
{
  List *next, *prev;
  void *data;
};

typedef struct MBKeyboard       MBKeyboard;
typedef struct MBKeyboardLayout MBKeyboardLayout;
typedef struct MBKeyboardRow    MBKeyboardRow;
typedef struct MBKeyboardKey    MBKeyboardKey;
typedef struct MBKeyboardUI     MBKeyboardUI;

typedef enum 
{
  MBKeyboardKeyActionGlyph,
  MBKeyboardKeyActionXKeySym, 	/* 'specials' be converted into this */
  MBKeyboardKeyActionModifier,

} MBKeyboardKeyActionType;

typedef enum 
{
  MBKeyboardKeyFaceGlyph,
  MBKeyboardKeyFaceImage,

} MBKeyboardKeyFaceType;

typedef enum 
{
  MBKeyboardKeyStateNormal = 0,
  MBKeyboardKeyStateShifted,
  MBKeyboardKeyStateMod1,
  MBKeyboardKeyStateMod2,
  MBKeyboardKeyStateMod3,
  N_MBKeyboardKeyStateTypes
} 
MBKeyboardKeyStateType;


struct MBKeyboard
{
  MBKeyboardUI          *ui;
  List                  *layouts;
  
};

/**** UI ***********/

int
mb_kbd_ui_init(MBKeyboard *kbd);

/**** Keyboard ****/

void
mb_kbd_add_layout(MBKeyboard *kb, MBKeyboardLayout *layout);

MBKeyboardLayout*
mb_kbd_get_current_layout(MBKeyboard *kb);


/**** Layout ****/

MBKeyboardLayout*
mb_kbd_layout_new(MBKeyboard *kbd, const char *id);

void
mb_kbd_layout_append_row(MBKeyboardLayout *layout,
			 MBKeyboardRow    *row);

List*
mb_kbd_layout_rows(MBKeyboardLayout *layout);

/**** Rows ******/

MBKeyboardRow*
mb_kbd_row_new(MBKeyboard *kbd);

void
mb_kbd_row_append_key(MBKeyboardRow *row, MBKeyboardKey *key);

List*
mb_kdb_row_keys(MBKeyboardRow *row);

/**** Keys ******/

MBKeyboardKey*
mb_kbd_key_new(MBKeyboard *kbd);

Bool
mb_kdb_key_has_state(MBKeyboardKey           *key,
		     MBKeyboardKeyStateType   state);

void
mb_kbd_key_set_glyph_face(MBKeyboardKey           *key,
			  MBKeyboardKeyStateType   state,
			  const unsigned char     *glyph);

void
mb_kbd_key_set_image_face(MBKeyboardKey           *key,
			  MBKeyboardKeyStateType   state,
			  void                    *image);

MBKeyboardKeyFaceType
mb_kbd_key_get_face_type(MBKeyboardKey           *key,
			 MBKeyboardKeyStateType   state);

void
mb_kbd_key_set_char_action(MBKeyboardKey           *key,
			   MBKeyboardKeyStateType   state,
			   const unsigned char     *glyphs);

/*** Config *****/

int
mb_kbd_config_load(MBKeyboard *kbd, char *conf_file);


/**** Util *****/

#define streq(a,b)      (strcmp(a,b) == 0)
#define unless(x)       if (!(x))

void*
util_malloc0(int size);

void
util_fatal_error(char *msg);

/* Util list */

typedef void (*ListForEachCB) (void *data, void *userdata) ;

#define util_list_next(l) (l)->next
#define util_list_previous(l) (l)->prev

List*
util_list_alloc_item(void);

List*
util_list_get_last(List *list);

List*
util_list_get_first(List *list);

List*
util_list_append(List *list, void *data);

List*
util_list_foreach(List *list, ListForEachCB func, void *userdata);


#endif