# Simple script to generate the test function headers for mtsTimingTest

priorities = ('VeryLow', 'Low', 'Normal', 'High', 'VeryHigh')

# continuous
for priority in priorities:
	for affinity in (0, 1):
		print '    void TestContinuousPriority%sAffinity%s(void);' % (priority, affinity)

print

# periodic
for priority in priorities:
	for affinity in (0, 1):
		for run in ('DummyComputation', 'OsaSleep', 'OsaThreadSleep'):
			print '    void TestPeriodicPriority%sAffinity%sRun%s(void);' % (priority, affinity, run)
