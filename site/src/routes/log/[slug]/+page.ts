import { error } from '@sveltejs/kit';

import { logsBySlug } from '$lib/logs';

export function load({ params }) {
	const entry = logsBySlug[params.slug];

	if (!entry) {
		error(404, 'Log entry not found');
	}

	return { entry };
}
