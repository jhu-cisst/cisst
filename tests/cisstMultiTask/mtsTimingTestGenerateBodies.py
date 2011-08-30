# Simple script to generate the test function bodies for mtsTimingTest

def priority_enum(priority):
	if priority == 'VeryLow':
		return 'PRIORITY_VERY_LOW'
	elif priority == 'Low':
		return 'PRIORITY_LOW'
	elif priority == 'Normal':
		return 'PRIORITY_NORMAL'
	elif priority == 'High':
		return 'PRIORITY_HIGH'
	elif priority == 'VeryHigh':
		return 'PRIORITY_VERY_HIGH'

def run_enum(run):
	if run == 'DummyComputation':
		return 'DUMMY_COMPUTATION'
	elif run == 'OsaSleep':
		return 'OSA_SLEEP'
	elif run == 'OsaThreadSleep':
		return 'OSA_THREAD_SLEEP'

priorities = ('VeryLow', 'Low', 'Normal', 'High', 'VeryHigh')

# continuous
for priority in priorities:
	for affinity in (0, 1):
		name = 'ContinuousPriority%sAffinity%s' % (priority, affinity)
		print '''\
void mtsTimingTest::Test%s(void)
{
     TestContinuous("%s", %s, %s);
}''' % (name, name, priority_enum(priority), affinity)

print

# periodic
for priority in priorities:
	for affinity in (0, 1):
		for run in ('DummyComputation', 'OsaSleep', 'OsaThreadSleep'):
			name = 'PeriodicPriority%sAffinity%sRun%s' % (priority, affinity, run)
			print '''\
void mtsTimingTest::Test%s(void)
{
    TestPeriodic("%s", %s, %s, %s);
}''' % (name, name, priority_enum(priority), affinity, run_enum(run))
