from Atrinik import *
from Interface import Interface
from QuestManager import QuestManagerMulti
from Quests import LostMemories as quest

activator = WhoIsActivator()
me = WhoAmI()
msg = WhatIsMessage().strip().lower()
qm = QuestManagerMulti(activator, quest)
inf = Interface(activator, me)

def main():
	if not qm.started_part(1):
		if msg == "hi" or msg == "hey" or msg == "hello":
			inf.add_msg("There you are! I didn't wake you up when we arrived in Incuna, because it seemed like you needed a good rest. You certainly did - you've been sleeping for quite a while now.")
			inf.add_msg("At any rate, we did it - we managed to reach Incuna. We should resupply and head to Strakewood at last - but don't worry, I'll handle that and anything else we may need.")
			inf.add_msg("However... there's still the question of what to do about your memory loss. Say, has it improved?")
			inf.add_link("<a=:remember>I feel I'm starting to remember...</a>")
			inf.add_link("<a=:remember>Not really...</a>")

		elif msg == "remember":
			inf.add_msg("Well, at any rate, I think you should visit the local church priest")
			qm.start(1)

	elif qm.completed():
		if msg == "hi" or msg == "hey" or msg == "hello":
			inf.add_msg("Ah, good! You seem much more confident now - almost the same person I have seen in you when you first came to me and hired me to transport you to Strakewood.")
			inf.add_msg("Very well then. Are you packed up and ready to go?")
			inf.add_link("I'm ready.")
			inf.add_link("<a=close:>Not just yet...</a>")

		elif msg == "i'm ready.":
			inf.close_dialog()

	else:
		if msg == "hi" or msg == "hey" or msg == "hello":
			inf.add_msg("Hmmm... You don't seem ready yet, {}. I am sorry, but unless you regain at least part of your memory, there's no reason for us to sail to Strakewood...".format(activator.name))

main()
inf.finish()
