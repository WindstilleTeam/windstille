class Coroutine
  def initialize()
    @continuation = nil
    @status = :waiting
  end

  def status()
    return @status
  end

  def update()
    if @status != :finished then
      @status = :running

      callcc {|@break|
        if @continuation then
          @continuation.call
        else
          self.run()
        end
        @status = :finished
      }
      
      if @status == :running then
        @status = :waiting
      end
    end
  end
  
  def restart()
    @status = :waiting
  end

  def wait()
    callcc {|@continuation| 
      @status = :waiting
      @break.call
    }
  end

  def waitTime(time)
    coroutine_wait(time)
    wait()
  end

  def waitFor(entity)
    #puts "waitFor: #{entity}"
    coroutine_waitFor(entity)
    wait()
    wait()
    wait()
    wait()
#    callcc {|@continuation| 
#      @status = :waiting
#      @break.call
#    }
  end

  def waitFrame()
    wait()
  end
end

# EOF #
