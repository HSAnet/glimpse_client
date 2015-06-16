


#include <ga-client.h>
#include<ga-client-enumtypes.h>

/* enumerations from "ga-client.h" */
GType
ga_client_state_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { GA_CLIENT_STATE_NOT_STARTED, "GA_CLIENT_STATE_NOT_STARTED", "GA_CLIENT_STATE_NOT_STARTED" },
      { GA_CLIENT_STATE_S_REGISTERING, "GA_CLIENT_STATE_S_REGISTERING", "GA_CLIENT_STATE_S_REGISTERING" },
      { GA_CLIENT_STATE_S_RUNNING, "GA_CLIENT_STATE_S_RUNNING", "GA_CLIENT_STATE_S_RUNNING" },
      { GA_CLIENT_STATE_S_COLLISION, "GA_CLIENT_STATE_S_COLLISION", "GA_CLIENT_STATE_S_COLLISION" },
      { GA_CLIENT_STATE_FAILURE, "GA_CLIENT_STATE_FAILURE", "GA_CLIENT_STATE_FAILURE" },
      { GA_CLIENT_STATE_CONNECTING, "GA_CLIENT_STATE_CONNECTING", "GA_CLIENT_STATE_CONNECTING" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("GaClientState", values);
  }
  return etype;
}
GType
ga_client_flags_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { GA_CLIENT_FLAG_NO_FLAGS, "GA_CLIENT_FLAG_NO_FLAGS", "GA_CLIENT_FLAG_NO_FLAGS" },
      { GA_CLIENT_FLAG_IGNORE_USER_CONFIG, "GA_CLIENT_FLAG_IGNORE_USER_CONFIG", "GA_CLIENT_FLAG_IGNORE_USER_CONFIG" },
      { GA_CLIENT_FLAG_NO_FAIL, "GA_CLIENT_FLAG_NO_FAIL", "GA_CLIENT_FLAG_NO_FAIL" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("GaClientFlags", values);
  }
  return etype;
}



