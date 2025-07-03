$$
\text{must}_p = 
\begin{cases}
\displaystyle \bigvee_{x\in{\text{son}}_p} \text{must}_x, & \text{if } x \text{ is not E} \\
1, & \text{if } x = \text{is E} \\
\end{cases}
$$

$$
\text{dp}_p = w_p + \displaystyle \sum_{x\in{\text{son}}_p}
\begin{cases}
\text{dp}_x, & \text{if } \text{must}_x \\
\max(\text{dp}_x, 0), & \text{else} \\
\end{cases}
$$