


#include <ga-entry-group.h>
#include<ga-entry-group-enumtypes.h>

/* enumerations from "ga-entry-group.h" */
GType
ga_entry_group_state_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { GA_ENTRY_GROUP_STATE_UNCOMMITED, "GA_ENTRY_GROUP_STATE_UNCOMMITED", "GA_ENTRY_GROUP_STATE_UNCOMMITED" },
      { GA_ENTRY_GROUP_STATE_REGISTERING, "GA_ENTRY_GROUP_STATE_REGISTERING", "GA_ENTRY_GROUP_STATE_REGISTERING" },
      { GA_ENTRY_GROUP_STATE_ESTABLISHED, "GA_ENTRY_GROUP_STATE_ESTABLISHED", "GA_ENTRY_GROUP_STATE_ESTABLISHED" },
      { GA_ENTRY_GROUP_STATE_COLLISTION, "GA_ENTRY_GROUP_STATE_COLLISTION", "GA_ENTRY_GROUP_STATE_COLLISTION" },
      { GA_ENTRY_GROUP_STATE_FAILURE, "GA_ENTRY_GROUP_STATE_FAILURE", "GA_ENTRY_GROUP_STATE_FAILURE" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("GaEntryGroupState", values);
  }
  return etype;
}



