# Seminar Site (Hugo)

This folder contains a Hugo-based bachelor seminar progress website.

## 1) Local run

```bash
cd seminar-site
hugo server -D
```

Open http://localhost:1313/

## 2) Create a new weekly log entry

```bash
cd seminar-site
hugo new --kind log log/2026-03-08-week-03.md
```

Then edit the created file in `content/log/`.

Weekly cadence rule used in this site:
- entries are dated every Sunday,
- Week 1 starts on 2026-02-22 (first Sunday after 16.2).

## 3) Build static output

```bash
cd seminar-site
hugo --minify
```

Build output is generated in `seminar-site/public/`.

## 4) GitHub Pages deploy

Deployment is automated by GitHub Actions using `.github/workflows/hugo-pages.yml`.

First, set your GitHub username in `seminar-site/hugo.toml`:

```toml
baseURL = "https://<username>.github.io/knapsack_cryptosystems/"
```

```bash
git add seminar-site .github/workflows/hugo-pages.yml
git commit -m "Add Hugo seminar site with Pages workflow"
git push origin main
```

Then in GitHub repository settings:
- Go to **Settings -> Pages**
- Ensure **Build and deployment** source is **GitHub Actions**
