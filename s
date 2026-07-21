document.addEventListener("DOMContentLoaded", function () {
    const filterAllBtn = document.getElementById("filter-all-notifications");
    const filterUnreadBtn = document.getElementById("filter-unread-notifications");

    // --- NOTIFICATION FILTER LOGIC ---
    if (filterAllBtn && filterUnreadBtn) {
        
        filterAllBtn.addEventListener("click", function () {
            document.querySelectorAll(".notification-card").forEach(card => {
                card.style.display = "flex";
            });
            filterAllBtn.style.background = "#1877f2";
            filterAllBtn.style.color = "#fff";
            filterUnreadBtn.style.background = "#f0f2f5";
            filterUnreadBtn.style.color = "#050505";
        });

        filterUnreadBtn.addEventListener("click", function () {
            document.querySelectorAll(".notification-card").forEach(card => {
                if (card.classList.contains("unread")) {
                    card.style.display = "flex";
                } else {
                    card.style.display = "none";
                }
            });
            filterUnreadBtn.style.background = "#1877f2";
            filterUnreadBtn.style.color = "#fff";
            filterAllBtn.style.background = "#f0f2f5";
            filterAllBtn.style.color = "#050505";
        });
    }

    // --- OPTIMIZED BADGE REFRESH ENGINE ---
    function updateNotificationBadge() {
        fetch('/notifications/count/')
            .then(response => {
                if (!response.ok) throw new Error("Network status invalid");
                return response.json();
            })
            .then(data => {
                const badge = document.getElementById("notification-badge");
                if (badge && data.count !== undefined) {
                    if (data.count > 0) {
                        badge.innerText = data.count;
                        badge.style.display = "inline-flex";
                    } else {
                        badge.style.display = "none";
                    }
                }
            })
            .catch(err => console.debug("Polling request paused or dropped"));
    }

    // Run once immediately when the page finishes loading
    updateNotificationBadge();

    // Run every 60 seconds, but ONLY if the user is actively viewing this browser tab
    setInterval(function() {
        if (!document.hidden) { 
            updateNotificationBadge();
        }
    }, 60000); 

    // Instantly refresh the count if a user leaves and returns to your website tab
    document.addEventListener("visibilitychange", function() {
        if (!document.hidden) {
            updateNotificationBadge();
        }
    });
});
