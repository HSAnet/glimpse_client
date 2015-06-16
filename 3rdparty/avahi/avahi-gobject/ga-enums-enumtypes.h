


#ifndef __GA_ENUMS_ENUM_TYPES_H__
#define __GA_ENUMS_ENUM_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS
/* enumerations from "ga-enums.h" */
GType ga_protocol_get_type (void);
#define GA_TYPE_PROTOCOL (ga_protocol_get_type())
GType ga_lookup_result_flags_get_type (void);
#define GA_TYPE_LOOKUP_RESULT_FLAGS (ga_lookup_result_flags_get_type())
GType ga_lookup_flags_get_type (void);
#define GA_TYPE_LOOKUP_FLAGS (ga_lookup_flags_get_type())
GType ga_resolver_event_get_type (void);
#define GA_TYPE_RESOLVER_EVENT (ga_resolver_event_get_type())
GType ga_browser_event_get_type (void);
#define GA_TYPE_BROWSER_EVENT (ga_browser_event_get_type())
G_END_DECLS

#endif /* __GA_ENUMS_ENUM_TYPES_H__ */



