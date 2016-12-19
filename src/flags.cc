#include <gflags/gflags.h>

// for ellet config
DEFINE_string(ellet_endpoint, "127.0.0.1:9527", "config the ip and port that ellet serves for");
DEFINE_string(ellet_segment_dir, "/tmp/", "config the segment dir for storaging data");

// for master config
DEFINE_string(elmaster_endpoint, "127.0.0.1:8527", "config the ip and port that elmaster servers for");

