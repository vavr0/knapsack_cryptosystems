<script lang="ts">
	let { data } = $props();
</script>

<article class="panel">
	<h1>{data.entry.title}</h1>
	<p class="meta">{data.entry.date.split('-').reverse().join('.')}</p>

	<section class="log-sections">
		<section class="log-section">
			<h2>Completed work</h2>
			<ul>
				{#each data.entry.completed as item}
					<li>{item}</li>
				{/each}
			</ul>
		</section>

		<section class="log-section">
			<h2>Next steps</h2>
			<ul>
				{#each data.entry.nextSteps as item}
					<li>{item}</li>
				{/each}
			</ul>
		</section>
	</section>
</article>
