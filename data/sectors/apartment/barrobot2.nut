class Character
{
  x = null;
  y = null;
  r = null;
  g = null;
  b = null;

  constructor(x_, y_, r_, g_, b_) {
    x = x_;
    y = y_;

    r = r_;
    g = g_;
    b = b_;
  }
  
  function say(text) {
    ::wait_for_speech(::speech_show(text, x, y, r, g, b));
  }
};

function init()
{
  robot <- Character(850, 160, 1.0, 0.0, 1.0);
  jane  <- Character(740, 120, 1.0, 1.0, 1.0);
}

function run()
{
  robot.say("I am robot.");
  robot.say("How can I help you?");
  jane.say("Hi there, cute little robot you are.");
  jane.say("I could use a little help when it comes to ...");
  switch(conversation([["Yagor", "That guy is getting on my nerves."],
                       ["Jeff",  "That idiot"]]))
    {
      case 0:
        function foo() {
          robot.say("Yagor, is my boss as much as yours.");
          jane .say("I don't take orders from anybody.");
          robot.say("But he is the one that has your paycheck.");
          jane .say("Good point, but aren't we in the future, where we no longer need those?");
          robot.say("This ain't the future of the fifthies...");
          robot.say("..this is real life.");
          jane .say("I don't think so.");
          robot.say("How can you say that?");
          jane .say("Its a videogame after all, isn't it.");
          robot.say("Guess so.");
          robot.say("Nice I could help.");
        }
        foo();
        break;

      case 1:
        robot.say("I see.");
        robot.say("Your profesional relationship doesn't seem to be all that professional after all.");
        jane.say("You bet. That guy is a dork");
        robot.say("Be easy on him, it might help you in the long run");
        jane.say("Will try");
        robot.say("Nice that I could help you");
        break;
    }
}

/* EOF */
