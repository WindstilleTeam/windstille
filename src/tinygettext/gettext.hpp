#ifndef __GETTEXT_H__
#define __GETTEXT_H__

#include "tinygettext/tinygettext.hpp"

extern tinygettext::DictionaryManager* dictionaryManager;

static inline const char* _(const char* message)
{
  if (dictionaryManager)
    return dictionaryManager->get_dictionary().translate(message);
  else
    return message;
}
#define N_(s)      s

#endif
