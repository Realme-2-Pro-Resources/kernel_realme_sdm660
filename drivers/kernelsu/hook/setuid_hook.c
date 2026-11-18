#ifdef CONFIG_KSU_SUSFS
extern struct work_struct susfs_extra_works;

static inline void ksu_handle_extra_susfs_work(void)
{
	if (work_pending(&susfs_extra_works))
		return;

	schedule_work(&susfs_extra_works);
}
#endif

static __always_inline void ksu_handle_setresuid_cred(struct cred *new, const struct cred *old)
{
	if (!new || !old)
		return;

	uid_t new_uid = ksu_get_uid_t(new->uid);
	uid_t old_uid = ksu_get_uid_t(old->uid);

	// old process is not root, ignore it.
	if (unlikely(!!old_uid))
		return;

	if (IS_ENABLED(CONFIG_KSU_DEBUG))
		pr_info("handle_setresuid from %d to %d\n", old_uid, new_uid);

	// we dont have those new fancy things upstream has
	// lets just do the original thing where we disable seccomp
	if (unlikely(is_uid_manager(new_uid)))
		goto install_ksu_fd;

	if (ksu_is_allow_uid_for_current(new_uid)) {
#ifdef CONFIG_KSU_SUSFS
		susfs_clear_current_proc_no_su();
		susfs_clear_current_proc_umounted();
#endif
		goto kill_seccomp;
	}

#ifdef CONFIG_KSU_SUSFS
	susfs_set_current_proc_no_su();
	susfs_set_current_proc_umounted();
#endif
	// Handle kernel umount
	ksu_handle_umount(new, old);
#ifdef CONFIG_KSU_SUSFS
	ksu_handle_extra_susfs_work();
#endif
	return;

install_ksu_fd:
	pr_info("install fd for manager: %d\n", new_uid);
	ksu_install_fd();

kill_seccomp:
	disable_seccomp();
	set_thread_flag(TIF_KSU_MANAGED); // sucompat fast-path
	return;
}

#ifdef CONFIG_KSU_SUSFS
extern u32 susfs_zygote_sid;
extern u32 susfs_zygote_next_sid;

int ksu_handle_setresuid(uid_t ruid, uid_t euid, uid_t suid)
{
	uid_t cur_uid = current_uid().val;

	if (cur_uid != 0)
		return 0;

	if (susfs_is_sid_equal(current_cred(), susfs_zygote_sid) ||
	    susfs_is_sid_equal(current_cred(), susfs_zygote_next_sid)) {
		if (is_isolated_process(ruid)) {
			susfs_set_current_proc_no_su();
			susfs_set_current_proc_umounted();
			ksu_handle_extra_susfs_work();
			return 0;
		}
		if (likely(ksu_is_manager_appid_valid()) && unlikely(is_uid_manager(ruid))) {
			disable_seccomp();
			pr_info("install fd for manager: %d\n", ruid);
			ksu_install_fd();
			return 0;
		}
		if (likely(is_appuid(ruid) && ksu_uid_should_umount(ruid))) {
			susfs_set_current_proc_no_su();
			susfs_set_current_proc_umounted();
			ksu_handle_extra_susfs_work();
			return 0;
		}
		if (ksu_is_allow_uid_for_current(ruid)) {
			disable_seccomp();
			susfs_clear_current_proc_no_su();
			susfs_clear_current_proc_umounted();
			return 0;
		}
		susfs_set_current_proc_no_su();
	}

	return 0;
}
#endif
