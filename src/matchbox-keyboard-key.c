#include "matchbox-keyboard.h"

#define MBKB_N_KEY_STATES 5

typedef struct MBKeyboardKeyFace
{
  MBKeyboardKeyFaceType  type;

  union
  {
    void          *image;
    unsigned char *str;
  } u;
} 
MBKeyboardKeyFace;

typedef struct MBKeyboardKeyAction
{
  MBKeyboardKeyActionType  type;
  union 
  {
    unsigned char         *glyph;
    KeySym                 keysym;
    MBKeyboardKeyModType   type;
  } u;
} 
MBKeyboardKeyAction;


/* A key can have 5 different 'states' */

typedef struct MBKeyboardKeyState
{
  MBKeyboardKeyAction action;
  MBKeyboardKeyFace   face;

} MBKeyboardKeyState;

struct MBKeyboardKey
{
  MBKeyboard            *kbd;
  MBKeyboardKeyState    *states[N_MBKeyboardKeyStateTypes];
  MBKeyboardRow         *row;

  int                    alloc_x, alloc_y, alloc_width, alloc_height;

  boolean                obeys_caps;
  boolean                fill;	     /* width fills avialble space */
  int                    req_uwidth; /* unit width in 1/1000's */
  boolean                is_blank;   /* 'blank' keys are spacers */

  MBKeyboardStateType    sets_kbdstate; /* needed */
};

static void
_mb_kbd_key_init_state(MBKeyboardKey           *key,
		       MBKeyboardKeyStateType   state)
{
  key->states[state] = util_malloc0(sizeof(MBKeyboardKeyState));
}

MBKeyboardKey*
mb_kbd_key_new(MBKeyboard *kbd)
{
  MBKeyboardKey *key = NULL;
  int            i;

  key      = util_malloc0(sizeof(MBKeyboardKey));
  key->kbd = kbd;

  for (i=0; i<N_MBKeyboardKeyStateTypes; i++)
    key->states[i] = NULL;

  return key;
}

void
mb_kbd_key_set_obey_caps(MBKeyboardKey  *key, boolean obey)
{
  key->obeys_caps = obey;
}

boolean
mb_kbd_key_get_obey_caps(MBKeyboardKey  *key)
{
  return key->obeys_caps;
}

void
mb_kbd_key_set_req_uwidth(MBKeyboardKey  *key, int uwidth)
{
  key->req_uwidth = uwidth;
}

int
mb_kbd_key_get_req_uwidth(MBKeyboardKey  *key)
{
  return key->req_uwidth;
}

void
mb_kbd_key_set_fill(MBKeyboardKey  *key, boolean fill)
{
  key->fill = fill;
}

boolean
mb_kbd_key_get_fill(MBKeyboardKey  *key)
{
  return key->fill;
}

void
mb_kbd_key_set_blank(MBKeyboardKey  *key, boolean blank)
{
  key->is_blank = blank;
}

boolean
mb_kbd_key_is_blank(MBKeyboardKey  *key)
{
  return key->is_blank;
}

void
mb_kbd_key_set_geometry(MBKeyboardKey  *key,
			int x,
			int y,
			int width,
			int height)
{
  if (x != -1)
    key->alloc_x = x;

  if (y != -1)
    key->alloc_y = y;

  if (width != -1)
    key->alloc_width = width;

  if (height != -1)
    key->alloc_height = height;
}

int 
mb_kbd_key_abs_x(MBKeyboardKey *key) 
{ 
  return mb_kbd_row_x(key->row) + key->alloc_x;
}

int 
mb_kbd_key_abs_y(MBKeyboardKey *key) 
{ 
  return mb_kbd_row_y(key->row) + key->alloc_y;
}

int 
mb_kbd_key_x(MBKeyboardKey *key) 
{ 
  return key->alloc_x;
}

int 
mb_kbd_key_y(MBKeyboardKey *key) 
{ 
  return key->alloc_y;
}

int 
mb_kbd_key_width(MBKeyboardKey *key) 
{ 
  return key->alloc_width;
}

int 
mb_kbd_key_height(MBKeyboardKey *key) 
{ 
  return key->alloc_height;
}

/* URG Nasty - some stuf should be public-ish */
void 
mb_kbd_key_set_row(MBKeyboardKey *key, MBKeyboardRow *row) 
{ 
  key->row = row;
}


boolean
mb_kdb_key_has_state(MBKeyboardKey           *key,
		     MBKeyboardKeyStateType   state)
{
  return (key->states[state] != NULL);
}

void
mb_kbd_key_set_glyph_face(MBKeyboardKey           *key,
			  MBKeyboardKeyStateType   state,
			  const unsigned char     *glyph)
{
  if (key->states[state] == NULL)
    _mb_kbd_key_init_state(key, state);

  key->states[state]->face.type    = MBKeyboardKeyFaceGlyph;
  key->states[state]->face.u.str   = strdup(glyph);
}

const unsigned char*
mb_kbd_key_get_glyph_face(MBKeyboardKey           *key,
			  MBKeyboardKeyStateType   state)
{
  if (key->states[state] 
      && key->states[state]->face.type == MBKeyboardKeyFaceGlyph)
    {
      return key->states[state]->face.u.str;
    }
  return NULL;
}

void
mb_kbd_key_set_image_face(MBKeyboardKey           *key,
			  MBKeyboardKeyStateType   state,
			  void                    *image)
{

  if (key->states[state] == NULL)
    _mb_kbd_key_init_state(key, state);

  key->states[state]->face.type    = MBKeyboardKeyFaceImage;
  key->states[state]->face.u.image = image;
}

void
mb_kbd_key_set_char_action(MBKeyboardKey           *key,
			   MBKeyboardKeyStateType   state,
			   const unsigned char     *glyphs)
{
  if (key->states[state] == NULL)
    _mb_kbd_key_init_state(key, state);
  
  key->states[state]->action.type = MBKeyboardKeyActionGlyph;
  key->states[state]->action.u.glyph = strdup(glyphs);
}

const unsigned char*
mb_kbd_key_get_char_action(MBKeyboardKey           *key,
			   MBKeyboardKeyStateType   state)
{
  if (key->states[state] 
      && key->states[state]->action.type == MBKeyboardKeyActionGlyph)
    return key->states[state]->action.u.glyph;

  return NULL;
}

void
mb_kbd_key_set_keysym_action(MBKeyboardKey           *key,
			     MBKeyboardKeyStateType   state,
			     KeySym                   keysym)
{
  if (key->states[state] == NULL)
    _mb_kbd_key_init_state(key, state);

  key->states[state]->action.type = MBKeyboardKeyActionXKeySym;
  key->states[state]->action.u.keysym = keysym;
}

KeySym
mb_kbd_key_get_keysym_action(MBKeyboardKey           *key,
			     MBKeyboardKeyStateType   state)
{
  if (key->states[state] 
      && key->states[state]->action.type == MBKeyboardKeyActionXKeySym)
    return key->states[state]->action.u.keysym;

  return None;
}


void
mb_kbd_key_set_modifer_action(MBKeyboardKey          *key,
			      MBKeyboardKeyStateType  state,
			      MBKeyboardKeyModType    type)
{
  if (key->states[state] == NULL)
    _mb_kbd_key_init_state(key, state);

  key->states[state]->action.type = MBKeyboardKeyActionModifier;
  key->states[state]->action.u.type   = type;
}

MBKeyboardKeyModType 
mb_kbd_key_get_modifer_action(MBKeyboardKey          *key,
			      MBKeyboardKeyStateType  state)
{
  if (key->states[state] 
      && key->states[state]->action.type == MBKeyboardKeyActionModifier)
    return key->states[state]->action.u.type;

  return 0;
}


MBKeyboardKeyFaceType
mb_kbd_key_get_face_type(MBKeyboardKey           *key,
			 MBKeyboardKeyStateType   state)
{
  if (key->states[state]) 
    return key->states[state]->face.type;

  return 0;
}


MBKeyboardKeyActionType
mb_kbd_key_get_action_type(MBKeyboardKey           *key,
			   MBKeyboardKeyStateType   state)
{
  if (key->states[state]) 
    return key->states[state]->action.type;

  return 0;
}


void
mb_kbd_key_press(MBKeyboardKey *key)
{
  /* XXX what about state handling XXX */
  MBKeyboardKeyStateType state = MBKeyboardKeyStateNormal;

  if (mb_kbd_key_is_blank(key))
    return;

  switch (mb_kbd_key_get_action_type(key, state))
    {
    case MBKeyboardKeyActionGlyph:
      {
	const unsigned char *key_char;

	if ((key_char = mb_kbd_key_get_char_action(key, state)) != NULL)
	  mb_kbd_ui_send_press(key->kbd->ui, key_char, 0);
	break;
      }
    case MBKeyboardKeyActionXKeySym:
      {
	/*
	KeySym ks;
	if ((ks = mb_kbd_key_get_keysym_action(key, state)) != None)
	  mb_kbd_ui_send_keysym_press(key->kbd->ui, key_char, 0);

	*/
	break;
      }
    case MBKeyboardKeyActionModifier:
      {
	/*
	KeySym ks;
	if ((ks = mb_kbd_key_get_keysym_action(key, state)) != None)
	  mb_kbd_ui_send_keysym_press(key->kbd->ui, key_char, 0);

	*/
	break;
      }

    default:
      break;
    }      
}

void
mb_kbd_key_release(MBKeyboard *kbd)
{
  mb_kbd_ui_send_release(kbd->ui);
}

void
mb_kbd_key_dump_key(MBKeyboardKey *key)
{
  int i;

  char state_lookup[][32] =
    {
      /* MBKeyboardKeyStateNormal  */ "Normal",
      /* MBKeyboardKeyStateShifted */ "Shifted" ,
      /* MBKeyboardKeyStateMod1,    */ "Mod1" ,
      /* MBKeyboardKeyStateMod2,    */ "Mod2" ,
      /* MBKeyboardKeyStateMod3,    */ "Mod3" 
    };

  fprintf(stderr, "-------------------------------\n");
  fprintf(stderr, "Dumping info for key at %p\n", key);

  for (i = 0; i < N_MBKeyboardKeyStateTypes; i++)
    {
      if (mb_kdb_key_has_state(key, i))
	{
	  fprintf(stderr, "state : %s\n", state_lookup[i]);
	  fprintf(stderr, "\tface type %i", mb_kbd_key_get_face_type(key, i));

	  if (mb_kbd_key_get_face_type(key, i) == MBKeyboardKeyFaceGlyph)
	    {
	      fprintf(stderr, ", showing '%s'", 
		      mb_kbd_key_get_glyph_face(key , i));
	    }

	  fprintf(stderr, "\n");

	  fprintf(stderr, "\taction type %i\n", 
		  mb_kbd_key_get_action_type(key, i));

	}
    }

  fprintf(stderr, "-------------------------------\n");

}

