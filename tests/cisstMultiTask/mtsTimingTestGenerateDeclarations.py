# Simple script to generate the test function declarations for mtsTimingTest

priorities = ('VeryLow', 'Low', 'Normal', 'High', 'VeryHigh')

print '\t\t// Test continuous'
# continuous
for priority in priorities:
	for affinity in (0, 1):
		print '\t\tCPPUNIT_TEST(TestContinuousPriority%sAffinity%s);' % (priority, affinity)

print

print '\t\t// Test periodic'
# periodic
for priority in priorities:
	for affinity in (0, 1):
		for run in ('DummyComputation', 'OsaSleep', 'OsaThreadSleep'):
			print '\t\tCPPUNIT_TEST(TestPeriodicPriority%sAffinity%sRun%s);' % (priority, affinity, run)
