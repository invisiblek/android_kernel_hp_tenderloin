#ifndef __TENDERLOIN_H__
#define __TENDERLOIN_H__

#include <sound/soc.h>
#include <sound/soc-dapm.h>

typedef int (*dai_init_cb)(struct snd_soc_pcm_runtime *rtd);

#ifdef CONFIG_MACH_TENDERLOIN
int tenderloin_soc_dai_init(struct snd_soc_pcm_runtime *rtd);
void tenderloin_soc_card_fixup(struct snd_soc_card *card, dai_init_cb dai_init);
#else
static inline int tenderloin_soc_dai_init(struct snd_soc_pcm_runtime *rtd)
{
  return 0;
}

static inline void tenderloin_soc_card_fixup(struct snd_soc_card *card, int (*dai_init)(struct snd_soc_pcm_runtime *rtd))
{
}
#endif

#endif /* !__TENDERLOIN_H__ */
