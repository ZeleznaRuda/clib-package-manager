const licenseUrl = "https://raw.githubusercontent.com/rudolfmuller/cclm-library-manager/refs/heads/main/LICENSE"; fetch(licenseUrl) .then(response => { if (!response.ok) throw new Error("Failed to load license"); return response.text(); }) .then(text => { document.getElementById('license').textContent = text; }) .catch(err => { document.getElementById('license').textContent = err; });
document.querySelectorAll('.copy-btn').forEach(button => {
    button.addEventListener('click', async () => {
        const code = button.parentElement.querySelector('code');
        const text = code.innerText.trim();

        try {
            if (navigator.clipboard) {
                await navigator.clipboard.writeText(text);
            } else {
                // Fallback for older browsers
                const textarea = document.createElement('textarea');
                textarea.value = text;
                document.body.appendChild(textarea);
                textarea.select();
                document.execCommand('copy');
                document.body.removeChild(textarea);
            }

            const originalText = button.textContent;
            button.textContent = 'Copied!';
            setTimeout(() => button.textContent = originalText, 1500);

        } catch (err) {
            button.textContent = 'Error';
            setTimeout(() => button.textContent = 'Copy', 1500);
        }
    });
});