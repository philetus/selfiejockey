class Touch:

	def __init__(self, plr, dt, intd):
		self.pillar = plr
		self.date = dt
		self.initd = intd
		self.ended = None
		self.dips = [] # each dip is a list of [inited, ended] times in secs

	def __str__(self):
		return "<touch pillar='" + str(self.pillar) + "' date='" + self.strdate() + "' initd='" + str(self.initd) + "' ended='" + str(self.ended) + "' />"

	def strdate(self):
		return "-".join(str(d) for d in self.date)
def parse_date(dtstr):

	# date string format: 2016-11-15-14-08-05-426
	ts = [int(t) for t in dtstr.split('-')]
	date = tuple(ts[:3]) # tuple can be dict key
	secs = (ts[3] * 60 * 60) + (ts[4] * 60) + ts[5] + (ts[6] * 0.001)

	return date, secs

def parse_initd(intd, endd):
	dlta = endd - intd
	secs = str(int(intd) % 60)
	if len(secs) == 1:
		secs = "0" + secs
	mins = str((int(intd) / 60) % 60)
	if len(mins) == 1:
		mins = "0" + mins
	hours = str(int(intd) / (60 * 60))
	return ":".join([hours, mins, secs]), str(dlta)

touches = [[], [], [], [], []]
touchstaks = [[], [], [], [], []]

with open("card_touch_log.tsv") as tf:
	print(tf.readline()) # skip heading line

	for ln in tf:

		plr, cd, dtstr = ln.split("\t")
		plr = int(plr)
		cd = int(cd)
		dt, scs = parse_date(dtstr)

		if cd == 98: # b -> init touch
			tch = Touch(plr, dt, scs)
			touchstaks[plr].append(tch)

		elif cd == 97: # a -> end touch
			if len(touchstaks[plr]) > 0:
				lst = touchstaks[plr].pop()
				lst.ended = scs

				touches[lst.pillar].append(lst)

		elif cd == 100: # d -> init dip
			if len(touchstaks[plr]) > 0:
				lst = touchstaks[plr][-1]
				lst.dips.append([scs])

		elif cd == 99: # c -> end dip
			if len(touchstaks[plr]) > 0:
				lst = touchstaks[plr][-1]
				if len(lst.dips) > 0 and len(lst.dips[-1]) == 1:
					lst.dips[-1].append(scs)

		else:
			print("unexpected code:", cd)

with open("touch_report.csv", 'w') as rf:

	rf.write("pillar,\tday,\tstart,\tlength\n")
	
	for plr in range(5):

		rf.write("\n")

		for tch in touches[plr]:
			tm, dlta = parse_initd(tch.initd, tch.ended)
			rf.write(str(tch.pillar) + ",\t" + tch.strdate() + ",\t" + tm + ",\t" + dlta + "\n")
			# for dp in tch.dips:
			# 	tm, dlta = parse_initd(dp[0], dp[1])
			# 	rf.write("\t\tdip time: " + tm + " length " + dlta + " seconds\n")
