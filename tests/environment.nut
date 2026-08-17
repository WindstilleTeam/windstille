function custom_function()
{
    ::print("custom_function()\n");
}

const const_value = 10;

function dump_table(tbl)
{
    // collect and sort objects by type
    local objects = {};
    foreach(k, v in tbl) {
        local kind = type(v);
        if (!(kind in objects)) {
            objects[kind] <- [];
        }

        objects[kind].append(k);
    }

    foreach(k, arr in objects) {
        arr.sort()
    }

    // dump table
    foreach(k, arr in objects) {
        ::print("### " + k + "\n");
        foreach(v in arr) {
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
::print("# Squirrel Scripting Environment\n");

::print("## Root Table\n");
dump_table(getroottable())

::print("## Const Table\n");
dump_table(getconsttable())
}

main()

/* EOF */
