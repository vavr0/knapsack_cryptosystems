export type LogEntry = {
	slug: string;
	title: string;
	date: string;
	summary: string;
	completed: string[];
	nextSteps: string[];
};

export const logs: LogEntry[] = [
	{
		slug: 'week-03',
		title: 'Week 03 - Direction Update and Cleanup',
		date: '2026-03-08',
		summary:
			'Refactored the Makefile, cleaned up the code and site, and turned supervisor feedback into concrete thesis tasks.',
		completed: [
			'Refactored the Makefile to simplify build and run commands.',
			'Made small code and website cleanup updates.',
			'Reviewed supervisor feedback and translated it into concrete thesis tasks.'
		],
		nextSteps: [
			'Implement a parameter-based demonstration in code showing how the classical knapsack can be broken.',
			'Draft a section on knapsack variants, known attacks, and their weaknesses.',
			'Add explicit computational complexity notes to each method and attack.'
		]
	},
	{
		slug: '2026-03-01-week-02-seminar-prep',
		title: 'Week 02 - Seminar Prep',
		date: '2026-03-01',
		summary:
			'Prepared the seminar presentation, polished the site, fixed Pages routing issues, and improved weekly log readability.',
		completed: [
			'Prepared the seminar presentation for tomorrow.',
			'Polished seminar website content, structure, and navigation.',
			'Fixed GitHub Pages project-path routing and deployment issues.',
			'Improved weekly log formatting and readability.'
		],
		nextSteps: [
			'Implement a parameter-based demonstration in code showing how the classical knapsack can be broken.',
			'Draft a section on knapsack variants, known attacks, and their weaknesses.',
			'Add explicit complexity notes to each discussed method and attack.'
		]
	},
	{
		slug: '2026-02-22-week-01-api-refactor-research-setup',
		title: 'Week 01 - Setup',
		date: '2026-02-22',
		summary:
			'Cleaned up the API and module boundaries, refactored duplicated logic, and organized the initial cryptosystem references.',
		completed: [
			'Performed major API and codebase cleanup and refactor.',
			'Consolidated shared interfaces into common headers and aligned module boundaries.',
			'Refactored utility, system, bench, and CLI paths to improve maintainability.',
			'Organized initial references for knapsack cryptosystem background.'
		],
		nextSteps: [
			'Lock final API and module contracts after cleanup.',
			'Polish notes and narrative for seminar demo continuity.',
			'Prepare the Week 2 seminar-focused update.'
		]
	}
];

export const logsBySlug = Object.fromEntries(logs.map((entry) => [entry.slug, entry]));
