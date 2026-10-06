// Dump the Squirrel root and const tables (diagnostic / environment probe).

function custom_function()
{
  ::print("custom_function()\n");
}

// Appears in the const table
const const_value = 10;

// Local only — does not appear in any table
local local_value = 10;

// Appears in the root table
global_value <- 10;

function dump_table(tbl)
{
  // Collect and sort object names by type
  local objects = {};
  foreach (k, v in tbl) {
    local kind = type(v);
    if (!(kind in objects)) {
      objects[kind] <- [];
    }
    objects[kind].append(k);
  }

  foreach (k, arr in objects) {
    arr.sort();
  }

  foreach (k, arr in objects) {
    ::print("### " + k + "\n");
    foreach (v in arr) {
      if (k == "function") {
        local info = tbl[v].getinfos();
        if (!info.native) {
          ::print("* " + v + " (" + info.src + ")\n");
        } else {
          ::print("* " + v + " (<native>)\n");
        }
      } else if (k == "integer" || k == "float" || k == "boolean" || k == "string") {
        ::print("* " + v + " -> " + tbl[v] + "\n");
      } else {
        ::print("* " + v + "\n");
      }
    }
    ::print("\n");
  }
}

function main()
{
  ::print("# Squirrel Scripting Environment (" + _version_ + ")\n");

  ::print("## Root Table\n");
  dump_table(getroottable());

  ::print("## Const Table\n");
  dump_table(getconsttable());
}

main();

/* EOF */
