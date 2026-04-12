<script lang="ts">
	import { resolve } from '$app/paths';
	import { logs } from '$lib/logs';
</script>

<section class="panel">
	<h1>Weekly Log</h1>
	<p>Chronological weekly updates for seminar progress.</p>

	<ul class="post-list">
		{#each logs as entry}
			<li>
				<div>
					<a href={resolve('/log/[slug]', { slug: entry.slug })}>{entry.title}</a>
					<p class="list-summary">{entry.summary}</p>
				</div>
				<span class="meta">{entry.date.split('-').reverse().join('.')}</span>
			</li>
		{/each}
	</ul>
</section>
