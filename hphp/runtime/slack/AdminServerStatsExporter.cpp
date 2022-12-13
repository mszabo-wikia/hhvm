#include "hphp/runtime/server/admin-request-handler.h"
#include "hphp/runtime/server/transport.h"
#include "hphp/runtime/server/writer.h"

#include <sstream>
#include <string_view>

namespace HPHP {

/*
 * This class automatically registers a new endpoint
 * on AdminServer that exports all stats in ServiceData as
 * a JSON object. The returned JSON object has a single key
 * with all the statistics under it as key/value.
 *
 * Because ServiceData is a global, centralized stats registry,
 * the counters returned will be from all of HHVM. Examples are
 * memory information, VM statistics, server requests, ...
 *
 * Callers are expected to filter the relevant stats from the
 * JSON payload.
 */
struct AdminServerStatsExporter final : public AdminCommandExt {
  virtual std::string usage() override {
    return "/all-stats:\n"
        "                  Returns all stats in ServiceData\n"
      ;
  }

  virtual bool handleRequest(Transport* transport) override {
    if (transport->getCommand() != "all-stats") {
      return false;
    }

    std::ostringstream out;
    JSONWriter w(out);
    w.writeFileHeader();
    w.beginObject("Stats");

    std::map<std::string, int64_t> statsMap;
    ServiceData::exportAll(statsMap);

    for (const auto& it : statsMap) {
      const std::string& stats_name = it.first;

      w.writeEntry(stats_name.c_str(), it.second);
    }
    w.endObject("Stats");
    w.writeFileFooter();

    transport->replaceHeader("Content-Type","application/json");
    transport->sendString(out.str());
    return true;
  }
};

// Ensure this is constructed in the global namespace
// as the constructor registers the command.
static AdminServerStatsExporter s_admin_server_stats_exporter;

} // namespace HPHP
