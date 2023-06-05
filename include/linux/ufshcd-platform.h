#ifndef __UFSHCD_PLATFORM_H__
#define __UFSHCD_PLATFORM_H__

#define MAX_UFS_STORE_HBA 3
struct ufs_hba;
extern struct ufs_hba *ufs_store_hba[MAX_UFS_STORE_HBA];
bool storage_is_ufs(void);

#endif
