
#ifndef ___ga_signals_marshal_MARSHAL_H__
#define ___ga_signals_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:INT,ENUM,STRING,STRING,STRING,STRING,POINTER,INT,POINTER,INT (signals-marshal.list:1) */
extern void _ga_signals_marshal_VOID__INT_ENUM_STRING_STRING_STRING_STRING_POINTER_INT_POINTER_INT (GClosure     *closure,
                                                                                                    GValue       *return_value,
                                                                                                    guint         n_param_values,
                                                                                                    const GValue *param_values,
                                                                                                    gpointer      invocation_hint,
                                                                                                    gpointer      marshal_data);

/* VOID:INT,ENUM,STRING,STRING,STRING,UINT (signals-marshal.list:2) */
extern void _ga_signals_marshal_VOID__INT_ENUM_STRING_STRING_STRING_UINT (GClosure     *closure,
                                                                          GValue       *return_value,
                                                                          guint         n_param_values,
                                                                          const GValue *param_values,
                                                                          gpointer      invocation_hint,
                                                                          gpointer      marshal_data);

/* VOID:INT,ENUM,STRING,UINT,UINT,POINTER,INT,INT (signals-marshal.list:3) */
extern void _ga_signals_marshal_VOID__INT_ENUM_STRING_UINT_UINT_POINTER_INT_INT (GClosure     *closure,
                                                                                 GValue       *return_value,
                                                                                 guint         n_param_values,
                                                                                 const GValue *param_values,
                                                                                 gpointer      invocation_hint,
                                                                                 gpointer      marshal_data);

G_END_DECLS

#endif /* ___ga_signals_marshal_MARSHAL_H__ */

