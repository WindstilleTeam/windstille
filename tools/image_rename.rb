#!/usr/bin/ruby -w

$LOAD_PATH.push("/home/ingo/projects/pingus/svn/trunk/sexpr/")

require "sexpr.rb"

def get_nodes(path, sexpr)
  
end

if ARGV.empty? then
  puts "Usage: #{$0} FILENAME..."
else
  ARGV.each { |filename|
    sexpr = SExpr::SExpr::parse(File.new(filename).read(), true, true)
    sexpr.each{|s|
      s.write($stdout)
    }

    nodes = get_nodes(sexpr, [root(),
                              sym("windstille-sector"),
                              sym("layers"),
                              sym("layer"),
                              sym("objects"),
                              sym("decal"),
                              sym("path"),
                              type("string")])
    nodes.each{|expr|
      # process
    }
  }
end

# EOF #
