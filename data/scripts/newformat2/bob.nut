function intro()
{
  bob_seen_before = true;
  dialog.show("Hey, my name's bob.  Are you new around here?"); 
  conversation_add("Yes, I am.");
  conversation_add("Go away"); 
  if (!conversation_get())
  {
    ask_questions();
  }
  else
  {
    bye();
  }
}

function hello_again()
{
  dialog.show("Hello again");
  conversation_add("Hey.");
  conversation_add("Bye");
  if (!conversation_get())
    ask_questions();
  else
    bye();
}

function ask_questions()
{
  dialog.show("So what are you doing here?");
  conversation_add("I work as a mercenary.");
  conversation_add("Not much");
  if (!conversation_get())
  {
      bob_knows_you_mercenary = true;
      offer_job();
  }
  else
    bye();
}

function offer_job()
{
  dialog.show("Are you looking for some work?");
  conversation_add("I might be, if the offer is good enough.");
  conversation_add("No thanks.");
  if (!conversation_get()) {
    add_objective("Kill spider", "You must kill the spider at the end of the level");
      explain_job();
  }
  else
    bye();
}

function explain_job()
{
  dialog.show("Your job is to kill the spider at the end of the level ;)");
}

function bye()
{
  if (is_objective_given("Kill spider"))
    dialog.show("Feel free to come back if you need a job");
  else
    dialog.show("See you again some time");
}

dialog <- Dialog(0, "Bob", "images/portraits/bob.sprite");
if (is_objective_given("Kill spider"))
  explain_job();
else if (bob_knows_you_mercenary)
  offer_job();
else if (bob_seen_before)
  hello_again();
else
  intro();

/* EOF */
